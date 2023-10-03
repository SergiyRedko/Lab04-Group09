struct job {
    int id;
    int arrival;
    int length;

    int executionStarted;
    int executionEnded;

    struct job *next;
};