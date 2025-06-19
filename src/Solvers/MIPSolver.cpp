#include "TxnSP/Solvers/MIPSolver.h"

namespace TxnSP
{
    SolverOutput* MIPSolver::solve(const SolverInput& input)
    {
		double beg = std::chrono::steady_clock::now().time_since_epoch().count();
        Problem* prb = input.prb;

		if(prb->getJobNumber() <= prb->getMachineNumber())
        {
            double end = std::chrono::steady_clock::now().time_since_epoch().count();
            SolverOutput* res = new SolverOutput(prb, (end - beg) / 1000000000);
        }

        double M = 0;        
		int n = prb->getJobNumber();
		int m = prb->getMachineNumber();
        double* T = prb->getLengths();
        bool** conf = prb->getConflicts();

		for (int i = 0; i < n; i++)
		{
			if (M < T[i])
			{
				M = T[i];
			}
		}

		M *= n;

        SCIP* scip;
		SCIPcreate(&scip);
		SCIPincludeDefaultPlugins(scip);
		SCIPcreateProbBasic(scip, "transactionScheduling");
		SCIPsetIntParam(scip, "display/verblevel", 0);
		SCIPsetIntParam(scip, "lp/threads", 1);

		SCIP_VAR* z;
		SCIP_VAR** s = new SCIP_VAR* [n];
		SCIP_VAR*** x = new SCIP_VAR** [n];
		SCIP_VAR*** pre = new SCIP_VAR** [n];

		SCIP_CONS** cons1 = new SCIP_CONS* [n];
		SCIP_CONS** cons2 = new SCIP_CONS* [n];
		SCIP_CONS*** cons3 = new SCIP_CONS** [n];
		SCIP_CONS**** cons4 = new SCIP_CONS*** [n];
		SCIP_CONS*** cons5 = new SCIP_CONS** [n];

		SCIP_VAR*** vars1 = new SCIP_VAR** [n];
		SCIP_VAR*** vars2 = new SCIP_VAR** [n];
		SCIP_VAR**** vars3 = new SCIP_VAR*** [n];
		SCIP_VAR***** vars4 = new SCIP_VAR**** [n];
		SCIP_VAR**** vars5 = new SCIP_VAR*** [n];

		double coef1[2]{ 1, -1 };
		double* coef2 = new double[m];
		double coef3[3]{ 1, -M, -1 };
		double coef4[4]{ 1, 1, -1, -1 };
		double coef5[2]{ 1, 1 };

		for (int j = 0; j < m; j++)
		{
			coef2[j] = 1;
		}

		SCIPcreateVarBasic(scip, &z, "z", 0.0, SCIPinfinity(scip), 1.0, SCIP_VARTYPE_CONTINUOUS);
		SCIPaddVar(scip, z);

		for (int i = 0; i < n; i++)
		{
			SCIPcreateVarBasic(scip, &s[i], "s", 0.0, SCIPinfinity(scip), 0, SCIP_VARTYPE_CONTINUOUS);
			SCIPaddVar(scip, s[i]);

			x[i] = new SCIP_VAR * [m];
			pre[i] = new SCIP_VAR * [n];

			for (int j = 0; j < m; j++)
			{
				SCIPcreateVarBasic(scip, &x[i][j], "x", 0.0, 1.0, 0, SCIP_VARTYPE_BINARY);
				SCIPaddVar(scip, x[i][j]);
			}

			for (int k = 0; k < n; k++)
			{
				SCIPcreateVarBasic(scip, &pre[i][k], "pre", 0.0, 1.0, 0, SCIP_VARTYPE_BINARY);
				SCIPaddVar(scip, pre[i][k]);
			}
		}

		for (int i = 0; i < n; i++)
		{
			vars1[i] = new SCIP_VAR * [2] { z, s[i] };
			SCIPcreateConsLinear(scip, &cons1[i], "cons1", 2, vars1[i], coef1, T[i], SCIPinfinity(scip), 1, 0, 1, 1, 1, 1, 0, 0, 0, 0);
			SCIPaddCons(scip, cons1[i]);

			vars2[i] = new SCIP_VAR* [m];

			for (int j = 0; j < m; j++)
			{
				vars2[i][j] = x[i][j];
			}

			SCIPcreateConsLinear(scip, &cons2[i], "cons2", m, vars2[i], coef2, 1.0, 1.0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0);
			SCIPaddCons(scip, cons2[i]);

			cons3[i] = new SCIP_CONS * [n];
			cons4[i] = new SCIP_CONS * *[n];
			cons5[i] = new SCIP_CONS * [n];

			vars3[i] = new SCIP_VAR * *[n];
			vars4[i] = new SCIP_VAR * **[n];
			vars5[i] = new SCIP_VAR * *[n];

			for (int k = 0; k < n; k++)
			{
				vars3[i][k] = new SCIP_VAR * [3] { s[i], pre[i][k], s[k] };
				SCIPcreateConsLinear(scip, &cons3[i][k], "cons3", 3, vars3[i][k], coef3, T[k] - M, SCIPinfinity(scip), 1, 0, 1, 1, 1, 1, 0, 0, 0, 0);
				SCIPaddCons(scip, cons3[i][k]);

				cons4[i][k] = new SCIP_CONS * [m];
				vars4[i][k] = new SCIP_VAR * *[m];

				if (i > k)
				{
					for (int j = 0; j < m; j++)
					{
						vars4[i][k][j] = new SCIP_VAR * [4] { pre[i][k], pre[k][i], x[i][j], x[k][j] };
						SCIPcreateConsLinear(scip, &cons4[i][k][j], "cons4", 4, vars4[i][k][j], coef4, -1.0, SCIPinfinity(scip), 1, 0, 1, 1, 1, 1, 0, 0, 0, 0);
						SCIPaddCons(scip, cons4[i][k][j]);
					}

					if (conf[i][k])
					{
						vars5[i][k] = new SCIP_VAR * [2] {pre[i][k], pre[k][i]};
						SCIPcreateConsLinear(scip, &cons5[i][k], "cons5", 2, vars5[i][k], coef5, 1.0, 1.0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0);
						SCIPaddCons(scip, cons5[i][k]);
					}
				}
			}
		}

		SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE);
		SCIPsolve(scip);
		SCIP_STATUS status = SCIPgetStatus(scip);
		SCIP_SOL* sol = SCIPgetBestSol(scip);

		int* resx = new int[n];
		double* ress = new double[n];


		SCIPreleaseVar(scip, &z);

		for (int i = 0; i < n; i++)
		{
			ress[i] = SCIPgetSolVal(scip, sol, s[i]);
			SCIPreleaseVar(scip, s + i);
			SCIPreleaseCons(scip, cons1 + i);
			SCIPreleaseCons(scip, cons2 + i);
			delete[] vars1[i];
			delete[] vars2[i];

			for (int k = 0; k < n; k++)
			{
				SCIPreleaseCons(scip, cons3[i] + k);
				SCIPreleaseVar(scip, pre[i] + k);
				delete[] vars3[i][k];

				if (i > k)
				{
					for (int j = 0; j < m; j++)
					{
						SCIPreleaseCons(scip, cons4[i][k] + j);
						delete[] vars4[i][k][j];
					}

					if (conf[i][k])
					{
						SCIPreleaseCons(scip, cons5[i] + k);
						delete[] vars5[i][k];
					}
				}

				delete[] cons4[i][k];
				delete[] vars4[i][k];
			}

			for (int j = 0; j < m; j++)
			{
				SCIP_Real solx = SCIPgetSolVal(scip, sol, x[i][j]);

				if (solx + 0.5 > 1)
				{
					resx[i] = j;
				}

				SCIPreleaseVar(scip, x[i] + j);
			}

			delete[] x[i];
			delete[] pre[i];

			delete[] cons3[i];
			delete[] cons4[i];
			delete[] cons5[i];

			delete[] vars3[i];
			delete[] vars4[i];
			delete[] vars5[i];
		}

		SCIPfree(&scip);

		delete[] s;
		delete[] x;
		delete[] pre;

		delete[] cons1;
		delete[] cons2;
		delete[] cons3;
		delete[] cons4;
		delete[] cons5;

		delete[] vars1;
		delete[] vars2;
		delete[] vars3;
		delete[] vars4;
		delete[] vars5;

		delete[] coef2;		

		if (status == SCIP_STATUS_OPTIMAL)
		{
			double end = std::chrono::steady_clock::now().time_since_epoch().count();
			SolverOutput* res = new SolverOutput(prb, resx, ress, (end - beg) / 1000000000);
			delete[] resx;
			delete[] ress;
			return res;
		}
		else
		{
			delete[] resx;
			delete[] ress;
			
			return nullptr;
		}
    }
}