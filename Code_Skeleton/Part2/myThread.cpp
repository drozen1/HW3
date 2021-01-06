
#include "myThread.h"
#include <set>
myThread::myThread(uint threadId, pthread_cond_t *cond1, pthread_cond_t *cond2, pthread_mutex_t *m,
                   PCQueue<Job> *jobQueue) : Thread(threadId) {
    this->cond1 = cond1;
    this->cond2 = cond2;
    this->m = m;
    this->jobQueue = jobQueue;
}
/**
 *
 * @return 0 if not legal
 * if legal - return value
 * phase 1= current matrix1
 * phase 2 = next matrix2
 */
static uint retrive_value(int row_index,int column_index,Job& j, int phase ) {
    if (row_index < 0 || row_index >= j.num_of_rows ||
        column_index < 0 || column_index >= j.num_of_columns) {
        return 0;
    }
    if (phase == 1) {
        vector<vector<int>> matrix= *j.current;
        vector<int> line = matrix[row_index];
        return line[column_index];
    }
    vector<vector<int>> matrix= *j.next;
    vector<int> line = matrix[row_index];
    return line[column_index];
}

static uint num_of_neighboards(int row_index,int column_index,Job& j,int phase ) {
    uint counter=0;
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
            if(i!=0 & k!=0){
                if(retrive_value(row_index+i,column_index+k,j,phase)>0){
                    counter++;
                }
            }
        }
    }
    return counter;
}

static uint calc_avg(int row_index,int column_index,Job& j,int phase ) {
    uint counter=0;
    uint sum=0;
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
            if(i!=0 & k!=0){
                if(retrive_value(row_index+i,column_index+k,j,phase)>0){
                    counter++;
                    sum+=retrive_value(row_index+i,column_index+k,j,phase);
                }
            }
        }
    }
    return sum/counter;
}

static uint calc_dominate(int row_index,int column_index,Job& j,int phase ) {
    int array[3] = {0, 0, 0};
    int count = 0;
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
            if (i != 0 & k != 0) {
                if (retrive_value(row_index + i, column_index + k, j, phase) > 0) {
                    array[count] = retrive_value(row_index + i, column_index + k, j, phase);
                    count++;
                }
            }
        }
    }
    assert(count == 3);
    uint dominate = 0;
    if (array[0] == array[1] && array[0] == array[2]) {
        return array[0];
    } else if (array[0] == array[1] && array[0] != array[2]) {
        if (array[0] * 2 >= array[2]) {
            return array[0];
        } else {
            return array[2];
        }
    } else if (array[0] != array[1] && array[0] == array[2]) {
        if (array[0] * 2 >= array[1]) {
            return array[0];
        } else {
            return array[1];
        }
    } else if (array[0] != array[1] && array[1] == array[2]) {
        if (array[1] * 2 >= array[0]) {
            return array[1];
        } else {
            return array[0];
        }
    } else {
        for (int i = 0; i < 3; i++) {
            if (dominate < array[i]) {
                dominate = array[i];
            }
        }
        return dominate;
    }
}

static void write_to_matrix(int row_index,int column_index,Job& j,int phase,uint value){
    if(phase==1) {
        (*j.next)[row_index][column_index] = value;
    }
    else {
        (*j.current)[row_index][column_index] = value;
    }
}

static void do_phase_one(Job& job){
    for (int i = job.start_row; i < job.end_row; ++i) {
        for (int j = 0; j < job.num_of_columns; ++j) {
            uint num_of_live_neighbors= num_of_neighboards(i,j,job,1);
            if(num_of_live_neighbors==3 && retrive_value(i,j,job,1)==0){
                /////birth
                uint ret= calc_dominate(i,j,job,1);
                write_to_matrix(i,j,job,1, ret);
            }else {
                if ((num_of_live_neighbors == 2 || num_of_live_neighbors == 3)
                    && retrive_value(i, j, job, 1) != 0) {
                    /////survive
                    // put the value from current to next
                    uint ret=retrive_value(i, j, job, 1);
                    write_to_matrix(i,j,job,1, ret);
                }else{
                    ///DIE
                    // put 0 from current to next
                    write_to_matrix(i,j,job,1, 0);
                }
            }
        }
    }
}

void myThread::thread_workload() {
    while (true) {
        Job j = jobQueue->pop();
        if (j.is_exit) {
            return;
        }

        // phase 1
        do_phase_one(j);
        (*j.counter1)++;
        while (*j.counter1 != j.total_jobs) {
            pthread_cond_wait(cond1, m);
        }
        pthread_cond_broadcast(cond1);
        ///TODO: phase 2
        (*j.counter2)++;
        while (*j.counter2 != j.total_jobs) {
            pthread_cond_wait(cond2, m);
        }
        pthread_cond_broadcast(cond2);

    }


}






