#include <stdio.h>
#include <unistd.h>

#define NUM_TASKS num_tasks

static enum task_ids_e {
    TASK_1_ID
,   TASK_2_ID
} task_id = TASK_1_ID;

static enum task_1_states_e {
    TASK_1_INIT_STATE
,   TASK_1_HELLO_STATE
,   TASK_1_WORLD_STATE
} task_1_state = TASK_1_INIT_STATE;

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

static int (*task_1_state_machine[])(void) = {
    [TASK_1_INIT_STATE  ] = &task_1_init_state_callback
,   [TASK_1_HELLO_STATE ] = &task_1_hello_state_callback
,   [TASK_1_WORLD_STATE ] = &task_1_world_state_callback
};

static int task_1_run()
{
    return task_1_state_machine[task_1_state]();
}

static int task_2_run()
{
    static enum task_2_state_e {
        TASK_2_INIT_STATE
    ,   TASK_2_TOGGLE_STATE
    ,   TASK_2_WAIT_STATE
    } task_2_state = TASK_2_INIT_STATE;

    static unsigned int task_2_bit = 0;

    switch (task_2_state) {
        case TASK_2_INIT_STATE:
            printf("%s: initializing task 2\n", __func__);
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

static int (*tasks[])(void) = {
    [TASK_1_ID] = &task_1_run
,   [TASK_2_ID] = &task_2_run
};

static const unsigned int num_tasks = sizeof (tasks) / sizeof (*tasks);

int main (int argc, char const *argv[])
{
    printf("%s num tasks: %u\n", __func__, NUM_TASKS);
    while (1) {
        task_id = (task_id + 1) % NUM_TASKS;
        tasks[task_id]();
        sleep(1);
    }
    return 0;
}