#include "Game.hpp"


static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}
Game::Game(game_params g){
    my_game_parms=g;
     m_gen_num=g.n_gen; 			 // The number of generations to run
     m_thread_num=g.n_thread; 			 // Effective number of threads = min(thread_num, field_height)

    this->interactive_on=g.interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT
    this-> print_on=g.print_on; // A
}

void Game::_init_game() {
    //start the bord
    // Create game fields - Consider using utils:read_file, utils::split
    vector<string> lines;
    lines=utils::read_lines( my_game_parms.filename);
    num_of_row=lines.size();
    //the number for threads we need
    if(num_of_row<m_thread_num){
        m_thread_num=num_of_row;
    }
   for (int i=0;i<num_of_row;i++){
       matrix1.push_back(utils::split(lines[i], ' '));
   }
   num_of_columns= matrix1[0].size();
   matrix2=matrix1;
    int ret;
/* initialize a condition variable to its default value */
    ret = pthread_cond_init(&this->cond1, NULL);
    if (ret!=0){
        cout<<"BUGG: cond1_init fail"<<endl;
    }
    ret = pthread_cond_init(&this->cond2, NULL);
    if (ret!=0){
        cout<<"BUGG: cond2_init fail"<<endl;
    }
    ret = pthread_mutex_init(&this->mutex, NULL);
    if (ret!=0){
        cout<<"BUGG: mutex_init game fail"<<endl;
    }
    this->jobQueue= PCQueue<Job>();
    // Create & Start threads
   for(uint i=1;i<m_thread_num+1;i++){
       myThread* t= new myThread(i,&this->cond1,&this->cond2,&this->mutex,&this->jobQueue);
       t->start();
       m_threadpool.push_back(t);
   }
   //calc the num jobs frames and size od frame
   uint num_of_row_per_job= num_of_row/m_thread_num;
   uint num_of_row_per_last_job=num_of_row%m_thread_num;
   //the range is [ ) i.e [0,2) = 0,1
    this->counter1=0;
    this->counter2=0;
    for (int i = 0; i < this->m_thread_num; ++i) {
        if(i==this->m_thread_num-1){
            Job j = Job(&matrix1,&matrix2,num_of_row_per_job*i,num_of_row_per_job*(i+1)+num_of_row_per_last_job,
                        &this->counter1,&this->counter2,this->m_thread_num, false,num_of_row,num_of_columns);
            this->all_jobs.push_back(j);
        }else{
            Job j = Job(&matrix1,&matrix2,num_of_row_per_job*i,num_of_row_per_job*(i+1),&this->counter1,
                        &this->counter2,this->m_thread_num, false,num_of_row,num_of_columns);
            this->all_jobs.push_back(j);
        }
    }
	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	// Push jobs to queue
    for (int i = 0; i < this->all_jobs.size(); ++i) {
        this->jobQueue.push(all_jobs[i]);
    }
	// Wait for the workers to finish calculating
    pthread_mutex_lock(&this->mutex);
    while (this->counter2 != this->m_thread_num && this->counter1!=0) {
        pthread_cond_wait(&cond2, &mutex);
    }
    this->counter2=0;
    pthread_mutex_unlock(&this->mutex);

    if(curr_gen==this->m_gen_num-1){
        for(uint i=0;i<m_thread_num;i++){
            Job j = Job(&matrix1,&matrix2,0,0,&this->counter1,
                        &this->counter2,this->m_thread_num, true,num_of_row,num_of_columns);
            this->jobQueue.push(j);
        }
            }
            return;
//	    }
//	}
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade 
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here

	for (uint i = 0; i < m_thread_num; ++i) {
        m_threadpool[i]->join();
    }
    //TODO: valgrind
    for(uint i=0;i<m_thread_num;i++){
        delete(m_threadpool[i]);
    }
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;

		///print

        cout << u8"╔" << string(u8"═") * this->num_of_columns  << u8"╗" << endl;
        for (uint i = 0; i < this->num_of_row; ++i) {
            cout << u8"║";
            for (uint j = 0; j <this->num_of_columns; ++j) {
                if (this->matrix1[i][j] > 0)
                    cout << colors[this->matrix1[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") *this->num_of_columns << u8"╝" << endl;


        // Display for GEN_SLEEP_USEC micro-seconds on screen
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}

uint Game::thread_num() const {//Returns the effective number of running threads = min(thread_num, field_height)
    return this->m_thread_num;
}
const vector<double> Game::gen_hist() const{// Returns the generation timing histogram
    // Note: In your implementation, all m_thread_num threads must write to this structure.
  return m_gen_hist;
}
const vector<double> Game::tile_hist() const{// Returns the tile timing histogram
    return m_tile_hist; 	 // Shared Timing history for tiles: First (2 * m_gen_num) cells are the calculation durations for tiles in generation 1 and so on.

}

/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters.
 */





