#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

struct input {
	int process;
	int burst_t;
	int arrival_t;
	int priority;
};

#define NUM 5
#define Process 5
#define time_q 3
int count = NUM;

void merge(struct input arr[], int left, int mid, int right)
{
	struct input* sortarr = (struct input*)malloc(sizeof(struct input) * (right + 1));
	int left_index = left;
	int right_index = mid + 1;
	int i;

	int index = left; //left

	while (left_index <= mid && right_index <= right)
	{
		if (arr[left_index].arrival_t <= arr[right_index].arrival_t)
			sortarr[index] = arr[left_index++];

		else
			sortarr[index] = arr[right_index++];

		index++;
	}

	if (left_index > mid)
	{
		for (i = right_index; i <= right; i++, index++)
			sortarr[index] = arr[i];
	}
	else
	{
		for (i = left_index; i <= mid; i++, index++)
			sortarr[index] = arr[i];
	}

	for (i = left; i <= right; i++)
		arr[i] = sortarr[i];

	free(sortarr);
}

void merge_sort(struct input arr[], int left, int right)
{
	int mid;

	if (left < right)
	{
		mid = (left + right) / 2;

		merge_sort(arr, left, mid);
		merge_sort(arr, mid + 1, right);

		/* 정렬된 두 배열을 병합한다 */
		merge(arr, left, mid, right);
	}
}

void* FCFS(struct input data[NUM], struct input arr[NUM])
{
	struct input inputdata[NUM], temp;

	for (int i = 0; i < NUM; i++) // 배열 복사
	{
		inputdata[i] = data[i];
	}

	merge_sort(inputdata, 0, NUM-1); // 맨 처음 도착하는 process 정하기

	for (int i = 0; i < NUM; i++)
	{
		arr[i] = inputdata[i];
	}

	return arr;
}

void* SJF(struct input data[NUM], struct input arr[NUM])
{
	struct input inputdata[NUM], temp;
	int current = 0;

	for (int i = 0; i < NUM; i++)
	{
		inputdata[i] = data[i];
	}
	current = inputdata[0].arrival_t;

	for (int i = 1; i < NUM; i++)
	{
		if (inputdata[i].arrival_t < current)
		{
			temp = inputdata[i];
			inputdata[i] = inputdata[0];
			inputdata[0] = temp;
		}
	}

	for (int i = 0; i < NUM-1; i++)
	{
		for (int j = i+1; j < NUM; j++)
		{
			if ((inputdata[i].burst_t > inputdata[j].burst_t) && inputdata[j].arrival_t < current)
			{
				temp = inputdata[i];
				inputdata[i] = inputdata[j];
				inputdata[j] = temp;
			}
			
		}
		current += inputdata[i].burst_t;
	}

	for (int i = 0; i < NUM; i++)
	{
		arr[i] = inputdata[i];
	}

	return arr;
}

void* Priority(struct input data[NUM], struct input arr[NUM])
{
	struct input inputdata[NUM], temp;
	int current = 0;

	for (int i = 0; i < NUM; i++)
	{
		inputdata[i] = data[i];
	}
	merge_sort(inputdata, 0, NUM - 1);

	current = inputdata[0].arrival_t;

	for (int i = 0; i < NUM - 1; i++)
	{
		for (int j = i + 1; j < NUM; j++)
		{
			if ((inputdata[i].priority > inputdata[j].priority) && inputdata[j].arrival_t < current)
			{
				temp = inputdata[i];
				inputdata[i] = inputdata[j];
				inputdata[j] = temp;
			}
			else if ((inputdata[i].priority == inputdata[j].priority) && inputdata[j].arrival_t < current && inputdata[i].arrival_t < current)
			{
				if (inputdata[i].burst_t > inputdata[j].burst_t)
				{
					temp = inputdata[i];
					inputdata[i] = inputdata[j];
					inputdata[j] = temp;
				}
			}
		}
			current += inputdata[i].burst_t;
	}

	for (int i = 0; i < NUM; i++)
	{
		arr[i] = inputdata[i];
	}

	return arr;
}

void* RR(struct input data[NUM], struct input rr_arr[NUM]) //round robin
{
	struct input inputdata[NUM * 2], temp;
	int index = 0, total_b = 1, r_count = 0;
	count = NUM;

	for (int i = 0; i < NUM; i++)
	{
		inputdata[i] = data[i];
		total_b += inputdata[i].burst_t;
	}

	merge_sort(inputdata, 0, NUM - 1);
	int i = 0;

	while (index < total_b)
	{	
		if (inputdata[i].burst_t > time_q)
		{
			inputdata[NUM + r_count] = inputdata[i];
			inputdata[NUM + r_count].burst_t -= time_q;

			inputdata[i].burst_t = time_q;
			r_count++;
		}

		inputdata[i].burst_t = (inputdata[i].burst_t > time_q) ? time_q : inputdata[i].burst_t;

		index++;
	}

	for (int j = 0; j < NUM+r_count; j++)
	{
		rr_arr[j] = inputdata[j];
	}

	return rr_arr;
}

void* P_SJF(struct input data[NUM], struct input rr_arr[NUM * 3])
{
	struct input inputdata[NUM * 3], temp;
	int current = 0, cnt = NUM;
	count = NUM;

	for (int i = 0; i < NUM; i++)
	{
		inputdata[i] = data[i];
	}

	// arrival time으로 초기 정렬
	for (int i = 1; i < NUM; i++)
	{
		if (inputdata[i].arrival_t < current)
		{
			temp = inputdata[i];
			inputdata[i] = inputdata[0];
			inputdata[0] = temp;
		}
	}
	
	for (int i = 0; i < NUM - 1; i++)
	{
		for (int j = i + 1; j < NUM; j++)
		{
			if ((inputdata[i].burst_t > inputdata[j].burst_t) && inputdata[j].arrival_t < current)
			{
				temp = inputdata[i];
				inputdata[i] = inputdata[j];
				inputdata[j] = temp;
			}

		}
		current += inputdata[i].burst_t;
	}

	current = inputdata[0].burst_t;

	for (int i = 0; i < NUM - 1; i++)
	{
		for (int j = i + 1; j < NUM; j++)
		{
			if (inputdata[j].arrival_t <= current && inputdata[i].arrival_t <= current)
			{
				if (inputdata[j].burst_t < inputdata[i].burst_t) // 새로운 도착 프로세스의 burst time이 더 짧을 때에만 실행 변경
				{
					inputdata[i].burst_t -= inputdata[j].burst_t; // 현재 실행 중인 프로세스의 남은 시간 갱신
					current += inputdata[j].burst_t; // 현재 시간을 갱신

					inputdata[count] = inputdata[i]; // 남은 부분을 배열의 뒤에 추가해 다시 실행될 수 있도록 함

					temp = inputdata[i];
					inputdata[i] = inputdata[j];
					inputdata[j] = temp;
					count++;
					cnt++;
				}
			}
		}
	}

	for (int i = 0; i < count; i++)
	{
		rr_arr[i] = inputdata[i];
	}

	return rr_arr;
}

void* P_P(struct input data[NUM], struct input rr_arr[NUM*2])
{
	struct input inputdata[NUM * 2], temp;
	int sum = NUM, current = 0;
	count = NUM;

	for (int i = 0; i < NUM; i++)
	{
		inputdata[i] = data[i];
	}

	for (int i = 0; i < NUM - 1; i++)
	{
		for (int j = i + 1; j < NUM; j++)
		{
			
			if ((inputdata[i].priority > inputdata[j].priority) && inputdata[i].arrival_t < current && inputdata[j].arrival_t < current)
			{
				temp = inputdata[i];
				inputdata[i] = inputdata[j];
				inputdata[j] = temp;
			}
			else if (inputdata[i].priority == inputdata[j].priority && inputdata[i].burst_t > inputdata[j].burst_t)
			{
				temp = inputdata[i];
				inputdata[i] = inputdata[j];
				inputdata[j] = temp;
			}

			current += inputdata[i].burst_t;
		}
	}

	current = inputdata[0].burst_t;

	for (int i = 0; i < sum-1; i++)
	{
		if (count >= NUM * 2)
			break;

		for (int j = i + 1; j < sum; j++)
		{
			if (inputdata[j].arrival_t <= current && inputdata[i].arrival_t <= current)
			{
				if (inputdata[j].priority < inputdata[i].priority) // 새로운 도착 프로세스의 priority가 너 높을 때 실행 변경
				{
					inputdata[i].burst_t -= inputdata[j].burst_t; // 현재 실행 중인 프로세스의 남은 시간 갱신
					current += inputdata[j].burst_t; // 현재 시간을 갱신

					inputdata[sum] = inputdata[i]; // 남은 부분을 배열의 뒤에 추가해 다시 실행될 수 있도록 함

					temp = inputdata[i];
					inputdata[i] = inputdata[j];
					inputdata[j] = temp;
					count++;
					sum++;
				}
			}
		}
	}
	
	for (int i = 0; i < sum; i++)
	{
		rr_arr[i] = inputdata[i];
	}

	return rr_arr;
}

int main() {
	
	int choose, sum = 0, aver_wait = 0, aver_turn = 0;
	int turn_t[7], t_min;
	int wait_t[7], w_min;

	struct input data[NUM];
	struct input arr[NUM];
	struct input rr_arr[NUM * 2];

	printf("write %d times- Process number, CPU burst time, Arrival time, Priority in order.\n", NUM);

	for (int i = 0; i < NUM; i++)
	{
		scanf("%d %d %d %d",  &(data[i].process), &(data[i].burst_t), &(data[i].arrival_t), &(data[i].priority));
	}
	
	printf("\n");

	while ((getchar() != '\n')) 
	{

	}
	
	while (choose != 8)
	{
		printf("\nchoose Scheduling Algorithm.\n1 : FCFS\n2 : SJF\n3 : Priority\n4 : Round Robin\n5 : Preemptive_SJF\n6 : Preemptive_Priority\n7 : Show Process\n8 : Exit\n");

		scanf("%d", &choose);

		if (choose != 8)
			printf("\nYou selected %d.\n", choose);
		else if (choose < 1 || choose > 8)
			printf("\nYou selected wrong number.\n");
		else
		{
			printf("\nFinished.\n");
			break;
		}
		
		switch (choose)
		{
		case 1: //FCFS

			FCFS(data, arr);

			printf("FCFS\nOrder : ");

			for (int i = 0; i < NUM; i++)
			{
				printf("Process %d ", arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < NUM; i++)
			{
				printf("P%d(%d) ", arr[i].process, (arr[i].burst_t) + sum);
				
				sum += arr[i].burst_t;
				aver_turn += (sum - (arr[i].arrival_t));
				aver_wait += (sum - arr[i].burst_t - arr[i].arrival_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));

			turn_t[0] = ((float)aver_turn / Process);
			wait_t[0] = ((float)aver_wait / Process);

			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;

			break;

		case 2: //Shortest Job First
			SJF(data, arr);
			printf("Shortest Job First\nOrder : ");

			for (int i = 0; i < NUM; i++)
			{
				printf("Process %d ", arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < NUM; i++)
			{
				printf("P%d(%d) ", arr[i].process, (arr[i].burst_t) + sum);
				sum += arr[i].burst_t;
				aver_turn += (sum - (arr[i].arrival_t));
				aver_wait += (sum - arr[i].burst_t - arr[i].arrival_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));
			
			turn_t[1] = ((float)aver_turn / Process);
			wait_t[1] = ((float)aver_wait / Process);
			
			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;
			
			break;

		case 3: //Priority
			Priority(data, arr);

			printf("Priority\nOrder : ");

			for (int i = 0; i < NUM; i++)
			{
				printf("Process %d ", arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < NUM; i++)
			{
				printf("P%d(%d) ", arr[i].process, (arr[i].burst_t) + sum);
				sum += arr[i].burst_t;
				aver_turn += (sum - arr[i].arrival_t);
				aver_wait += (sum - arr[i].burst_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));

			turn_t[2] = ((float)aver_turn / Process);
			wait_t[2] = ((float)aver_wait / Process);

			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;

			break;

		case 4: //Round Robin
			RR(data, rr_arr);

			printf("Round Robin\nOrder : ");

			for (int i = 0; i < count; i++)
			{
				printf("Process %d ", rr_arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < count; i++)
			{
				printf("P%d(%d) ", rr_arr[i].process, (rr_arr[i].burst_t) + sum);
				sum += rr_arr[i].burst_t;
				aver_turn += (sum - rr_arr[i].arrival_t);
				aver_wait += (sum - rr_arr[i].burst_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));

			turn_t[3] = ((float)aver_turn / Process);
			wait_t[3] = ((float)aver_wait / Process);

			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;
			break;

		case 5: //Preemptive_SJF
			P_SJF(data, rr_arr);
			printf("Preemptive_Shortest Job First\nOrder : ");

			for(int i = 0; i < count; i++)
			{
				printf("Process %d ", rr_arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < count; i++)
			{
				printf("P%d(%d) ", rr_arr[i].process, sum + rr_arr[i].burst_t);
				sum += rr_arr[i].burst_t;
				aver_turn += (sum - rr_arr[i].arrival_t);
				aver_wait += (sum - rr_arr[i].burst_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));

			turn_t[4] = ((float)aver_turn / Process);
			wait_t[4] = ((float)aver_wait / Process);

			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;
			break;

		case 6 : // Preemptive_Priority
			P_P(data, rr_arr);

			printf("Preemptive_Priority\nOrder : ");

			for (int i = 0; i < count; i++)
			{
				printf("Process %d ", rr_arr[i].process);
			}

			printf("\n");

			for (int i = 0; i < NUM; i++)
			{
				printf("P%d(%d) ", rr_arr[i].process, (rr_arr[i].burst_t) + sum);
				sum += rr_arr[i].burst_t;
				aver_turn += (sum - arr[i].arrival_t);
				aver_wait += (sum - arr[i].burst_t);
			}

			printf("\nAverage Turnaround Time : %.2f\n", ((float)aver_turn / Process));
			printf("Average Waiting Time : %.2f\n", ((float)aver_wait / Process));

			turn_t[5] = ((float)aver_turn / Process);
			wait_t[5] = ((float)aver_wait / Process);

			sum = 0;
			aver_turn = 0, aver_wait = 0;
			count = NUM;
			break;

		case 7 : 
			
			for (int i = 0; i < NUM; i++)
			{
				printf("%d %d %d %d\n", data[i].process, data[i].burst_t, data[i].arrival_t, data[i].priority);
			}
			break;

		case 8: //exit

			break;
		}

	}

	for (int i = 0; i < 6; i++)
	{
		t_min = turn_t[0];
		if (turn_t[i] < t_min)
			t_min = turn_t[i];
	}

	for (int i = 0; i < 6; i++)
	{
		w_min = wait_t[0];
		if (wait_t[i] < w_min)
			w_min = wait_t[i];
	}

	switch (t_min)
	{
	case 1:
		printf("\nFCFS is the solution with best average turnaround time in this way.\n");
		break;
	case 2:
		printf("\nSJF is the solution with best average turnaround time in this way.\n");
		break;
	case 3:
		printf("\nPriority is the solution with best average turnaround time in this way.\n");
		break;
	case 4:
		printf("\nRound Robin is the solution with best average turnaround time in this way.\n");
		break;
	case 5:
		printf("\nPreemptive_SJF is the solution with best average turnaround time in this way.\n");
		break;
	case 6:
		printf("\nPreemptive_Priority is the solution with best average turnaround time in this way.\n");
		break;
	}

	switch (w_min)
	{
	case 1:
		printf("FCFS is the solution with best average waiting time in this way.\n");
		break;
	case 2:
		printf("SJF is the solution with best average waiting time in this way.\n");
		break;
	case 3:
		printf("Priority is the solution with best average waiting time in this way.\n");
		break;
	case 4:
		printf("Round Robin is the solution with best average waiting time in this way.\n");
		break;
	case 5:
		printf("Preemptive_SJF is the solution with best average waiting time in this way.\n");
		break;
	case 6:
		printf("Preemptive_Priority is the solution with best average waiting time in this way.\n");
		break;
	}

	return 0;
}
