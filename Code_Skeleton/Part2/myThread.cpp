
#include "myThread.h"
myThread::myThread(uint threadId, pthread_cond_t *cond1, pthread_cond_t *cond2, pthread_mutex_t *m,
                   PCQueue<Job> *jobQueue,vector<double>* m_tile_hist) : Thread(threadId) {
    this->cond1 = cond1;
    this->cond2 = cond2;
    this->m = m;
    this->jobQueue = jobQueue;
    this->m_tile_hist=m_tile_hist;
}
/**
 *
 * @return 0 if not legal
 * if legal - return value
 * phase 1= current matrix1
 * phase 2 = next matrix2
 */
static uint retrive_value(uint row_index,uint column_index,Job& j, int phase ) {
    if (row_index < 0 || row_index >= j.num_of_rows ||
        column_index < 0 || column_index >= j.num_of_columns) {
        return 0;
    }
    if (phase == 1) {
        return (*j.current)[row_index][column_index] ;
    }
    return (*j.next)[row_index][column_index] ;
}

static uint num_of_neighboards(uint row_index,uint column_index,Job& j,int phase ) {
    uint counter=0;
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
            if(i==0 && k==0) {
                continue;
            }else{
                    uint ret = retrive_value(row_index + i, column_index + k, j, phase);
                    if (ret > 0) {
                        counter++;
                    }
                }
            }
        }

    return counter;
}

static uint calc_avg(uint row_index,uint column_index,Job& j,int phase ) {
    uint counter=0;
    uint sum=0;
    //TODO: remove this line
    //assert(retrive_value(row_index,column_index,j,phase)>0);
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
                if(retrive_value(row_index+i,column_index+k,j,phase)>0){
                    counter++;
                    sum+=retrive_value(row_index+i,column_index+k,j,phase);
                }
            }
        }
    return round(double(sum)/double(counter));
}

static uint calc_dominate(uint row_index,uint column_index,Job& j,int phase ) {
    uint array[3] = {0, 0, 0};
    int count = 0;
    for (int i = -1; i < 2; ++i) {
        for (int k = -1; k < 2; ++k) {
            if (i == 0 && k == 0) {
                continue;
            } else {
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

static void write_to_matrix(uint row_index,uint column_index,Job& j,int phase,uint value){
    if(phase==1) {
        (*j.next)[row_index][column_index] = value;
    }
    else {
        (*j.current)[row_index][column_index] = value;
    }
}

static void do_phase_one(Job& job){
    for (uint i = job.start_row; i < job.end_row; ++i) {
        for (uint j = 0; j < job.num_of_columns; ++j) {
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
static void do_phase_two(Job& job) {
    for (uint i = job.start_row; i < job.end_row; ++i) {
        for (uint j = 0; j < job.num_of_columns; ++j) {
            if( retrive_value(i,j,job, 2 )>0){
                uint ret=calc_avg(i,j,job,2);
                write_to_matrix(i,j,job,2, ret);
            }else{
                write_to_matrix(i,j,job,2, 0);
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
        auto gen_start1 = std::chrono::system_clock::now();
        // phase 1
        do_phase_one(j);
        auto gen_end1 = std::chrono::system_clock::now();
        pthread_mutex_lock(m);
        this->m_tile_hist->push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end1 - gen_start1).count());
        (*j.counter1)++;
        if(*j.counter1 == j.total_jobs){
            pthread_cond_broadcast(cond1);
        }
        while (*j.counter1 != j.total_jobs) {
            pthread_cond_wait(cond1, m);
        }
        pthread_mutex_unlock(m);

        // phase 2
        auto gen_start2 = std::chrono::system_clock::now();
        do_phase_two( j);
        auto gen_end2 = std::chrono::system_clock::now();
        pthread_mutex_lock(m);
        this->m_tile_hist->push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end2 - gen_start2).count());
        (*j.counter2)++;
        if(*j.counter2 == j.total_jobs){
            pthread_cond_broadcast(cond2);
        }
        while (*j.counter2 != j.total_jobs) {
            pthread_cond_wait(cond2, m);
        }
        (*j.counter1)--;
        if(*j.counter1==0){
            //or signal
            *j.enter=true;
            pthread_cond_signal(cond2);
        }
        pthread_mutex_unlock(m);
    }
}






