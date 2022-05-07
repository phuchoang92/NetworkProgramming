
#include <stdio.h>
#include <windows.h>
#include <ctime>

static long num_steps = 1000000000;
double step, pi;

#define NUM_THREADS 8
double localSum[NUM_THREADS];

DWORD WINAPI ThreadCal(LPVOID lpParam) {
	int idx = *(int*) lpParam;

	double x, s = 0.0;


	for (int i = idx; i < num_steps; i+=NUM_THREADS)
	{
		x = (i + 0.5) * step;
		s = s + 4.0 / (1.0 + x*x);
	}

	localSum[idx] = s;
	
	return 0;
}

int main() {
	HANDLE hThread[NUM_THREADS];
	int params[NUM_THREADS];

	step = 1.0 / (double)num_steps;
	
	clock_t t;
	t = clock();


	for (int i = 0; i < NUM_THREADS; i++){
		localSum[i] = 0.0;
		params[i] = i;
		hThread[i] = CreateThread(0, 0, ThreadCal, &params[i], 0,0);
	}

	WaitForMultipleObjects(NUM_THREADS, hThread, TRUE, INFINITE);

	double sum = 0.0;


	for (int i = 0; i < NUM_THREADS; i++)
		sum += localSum[i];
	
	t = clock() - t;
	double elapsed = (double)t / CLOCKS_PER_SEC;

	pi = step * sum;

	printf("PI: %f", pi);
	printf("Time to finish: %f", elapsed);

}