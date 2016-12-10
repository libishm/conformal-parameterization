#include "Solver.h"
#include <eigen/SparseCholesky>
#define alpha 0.5
#define beta 0.9
#define EPSILON 1e-7

Solver::Solver(int n0):
n(n0)
{

}

void Solver::gradientDescent()
{
    double f = 0.0;
    x = Eigen::VectorXd::Zero(n);
    Eigen::VectorXd xp = x, v = x;
    handle->computeEnergy(f, x);

    int k = 1;
    while (true) {
        // compute momentum term
        v = x;
        if (k > 1) v += (k-2)*(x - xp)/(k+1);
        
        // compute update direction
        Eigen::VectorXd g(n);
        handle->computeGradient(g, v);
        
        // compute step size
        double t = 1.0;
        double fp = 0.0;
        Eigen::VectorXd xn = v - t*g;
        Eigen::VectorXd xnv = xn - v;
        handle->computeEnergy(fp, v);
        handle->computeEnergy(f, xn);
        while (f > fp + g.dot(xnv) + xnv.dot(xnv)/(2*t)) {
            t = beta*t;
            xn = v - t*g;
            xnv = xn - v;
            handle->computeEnergy(f, xn);
        }

        // update
        xp = x;
        x = xn;
        k++;
        
        // check termination condition
        if (fabs(f - fp) < EPSILON) break;
    }
}

void Solver::coordinateDescent()
{
    // TODO
}

void solve(Eigen::VectorXd& x, const Eigen::VectorXd& b, const Eigen::SparseMatrix<double>& A)
{
    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> solver(A);
    x = solver.solve(b);
}

void Solver::newton()
{   
    double f = 0.0;
    x = Eigen::VectorXd::Zero(n);
    handle->computeEnergy(f, x);

    while (true) {
        // compute update direction
        Eigen::VectorXd g(n);
        handle->computeGradient(g, x);

        Eigen::SparseMatrix<double> H(n, n);
        handle->computeHessian(H, x);

        Eigen::VectorXd p;
        solve(p, g, H);

        // compute step size
        double t = 1.0;
        double fp = f;
        handle->computeEnergy(f, x - t*p);
        while (f > fp - alpha*t*g.dot(p)) {
            t = beta*t;
            handle->computeEnergy(f, x - t*p);
        }

        // update
        x -= t*p;

        // check termination condition
        if (fabs(f - fp) < EPSILON) break;
    }
}

void Solver::lbfgs()
{
    // TODO
}