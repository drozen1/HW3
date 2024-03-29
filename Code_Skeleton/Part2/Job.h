//
// Created by student on 1/5/21.
//

#ifndef HW3_JOB_H
#define HW3_JOB_H


//
// Created by student on 1/5/21.
//
#include "../Part1/Headers.hpp"


class Job {
public:
    vector<vector<int>>* current;
    vector<vector<int>>* next;
    uint start_row, end_row;
    uint* counter1;
    uint* counter2;
    uint total_jobs;
    bool is_exit;
    uint num_of_rows;
    uint num_of_columns;
    bool* enter;

    Job( vector<vector<int>>* current,vector<vector<int>>* next, uint start,uint end,
         uint* counter1,uint* counter2, uint total_jobs, bool is_exit,uint num_of_row,uint num_of_columns,bool* enter) {
        this->current=current;
        this->next=next;
        this->start_row=start;
        this->end_row=end;
        this->counter1=counter1;
        this->counter2=counter2;
        this->total_jobs=total_jobs;
        this->is_exit=is_exit;
        this->num_of_columns=num_of_columns;
        this->num_of_rows=num_of_row;
        this->enter=enter;
    }

};
#endif //HW3_JOB_H
