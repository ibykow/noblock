/*

    noblock.c
    @Ilia Bykow
    5 Jan, 2014

    A simple, co-operative (non-blocking) multitasking example.

    License: Free for non-commercial use. Caveat emptor.

*/

#include <stdio.h>
#include <unistd.h> // for sleep function

/* INTERTASK_SLEEP_DURATION: Define how long we should sleep for in between
    running each task. */
#define INTERTASK_SLEEP_DURATION 1

/* NUM_TASKS: a helper define for the num_tasks variable defined further down. */
#define NUM_TASKS num_tasks

/* task_id: Keep track of which task we are currently running. */
static enum task_ids_e {
    TASK_1_ID
,   TASK_2_ID
} task_id = TASK_1_ID;

/* task_1_state: Keep track of state of task 1. */
static enum task_1_states_e {
    TASK_1_INIT_STATE
,   TASK_1_HELLO_STATE
,   TASK_1_WORLD_STATE
} task_1_state = TASK_1_INIT_STATE;

/* Each state task 1 state has a callback associated with it:
    'init', 'hello', and 'world' states. */
static int task_1_init_state_callback()
{
    printf("%s: initializing task 1\n", __func__);
    task_1_state = TASK_1_HELLO_STATE;
    return 0;
}

static int task_1_hello_state_callback()
{
    printf("%s: Hello,\n", __func__);
    task_1_state = TASK_1_WORLD_STATE;
    return 0;
}

static int task_1_world_state_callback()
{
    printf("%s: World!\n", __func__);
    task_1_state = TASK_1_HELLO_STATE;
    return 0;
}

/* task_1_state_machine: an array of pointers to functions.
    Associate each task 1 state with its corresponding callback. */
static int (*task_1_state_machine[])(void) = {
    [TASK_1_INIT_STATE  ] = &task_1_init_state_callback
,   [TASK_1_HELLO_STATE ] = &task_1_hello_state_callback
,   [TASK_1_WORLD_STATE ] = &task_1_world_state_callback
};

/* task_1_run: a helper function. Simply run the task 1 callback
    function associated with its current state.
    Eg. if the current state of task 1 is the 'hello' state, call
        the hello callback defined above. */
static int task_1_run()
{
    return task_1_state_machine[task_1_state]();
}

/* task_2_run: Task 2 is structured differently from task 1.
    State information is encapsulated within the function and we
    use a switch statement to determine which state to run next. */
static int task_2_run()
{
    static enum task_2_state_e {
        TASK_2_INIT_STATE
    ,   TASK_2_TOGGLE_STATE
    ,   TASK_2_WAIT_STATE
    } task_2_state = TASK_2_INIT_STATE;

    static unsigned int task_2_bit;

    switch (task_2_state) {
        case TASK_2_INIT_STATE:
            printf("%s: initializing task 2\n", __func__);
            task_2_bit = 0;
            task_2_state = TASK_2_TOGGLE_STATE;
            break;
        case TASK_2_TOGGLE_STATE:
            task_2_bit ^= 1;
            printf("%s: the bit is %s\n", __func__, task_2_bit ? "ON" : "OFF");
            task_2_state = TASK_2_WAIT_STATE;
            break;
        case TASK_2_WAIT_STATE:
            // Print nothing
            task_2_state = TASK_2_TOGGLE_STATE;
            break;
    }

    return 0;
}

/* tasks: an array of pointers to functions. Associated task ids with their
    respective task 'run' functions. */
static int (*tasks[])(void) = {
    [TASK_1_ID] = &task_1_run
,   [TASK_2_ID] = &task_2_run
};

/* num_tasks: calculate the number of tasks available for execution.
    Make this a const, because it's only calculated once and never has to be
    updated. If we wanted to add tasks dynamically, the 'const' qualifier
    would have to be removed. */
static const unsigned int num_tasks = sizeof (tasks) / sizeof (*tasks);

/* main: run each task in turn (round robin scheduling), and sleep for one
    second between each task call. */
int main (int argc, char const *argv[])
{
    /* print out some debugging information: how many tasks do we
        have in total? */
    printf("%s num tasks: %u\n", __func__, NUM_TASKS);

    /* main loop */
    while (1) {
        /* update the task ID. */
        task_id = (task_id + 1) % NUM_TASKS;

        /* Call the function associated with the task id. */
        tasks[task_id]();

        /* Sleep for some duration. */
        sleep(INTERTASK_SLEEP_DURATION);
    }

    /* We never get here, as the main loop should never exit.
        Keep this here anyway to make the compiler happy. */
    return 0;
}
