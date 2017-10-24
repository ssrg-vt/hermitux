#include <stdlib.h>
#include <assert.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>

static volatile int    alrm_triggered;

static void sig_alrm(int s)
{
       alrm_triggered = 1;
}

int main()
{
       int             rc;
       time_t          end;
       sighandler_t    old_sig;

       alarm(50);

       old_sig = signal(SIGALRM, &sig_alrm);
       assert(old_sig != SIG_ERR);

       /* check whether alarm() returns correct number of remaining
        * seconds */
       rc = alarm(2);
       assert(rc > 40 && rc <= 50);

       /* check whether SIGALRM is triggered within the set time */
       end = time(NULL) + 5;
       while (!alrm_triggered && time(NULL) < end) {
               /* noop */
       }
       assert(alrm_triggered);

       /* there should be no pending alarm */
       rc = alarm(0);
       assert(rc == 0);

       alrm_triggered = 0;

       /* test whether alarm can be canceled */
       rc = alarm(2);
       assert(rc == 0);

       rc = alarm(0);
       assert(rc > 0 && rc < 4);
       assert(!alrm_triggered);

       /* there should not happen an alarm */
       end = time(NULL) + 5;
       while (!alrm_triggered && time(NULL) < end) {
               /* noop */
       }
       assert(!alrm_triggered);

       /* there should be no pending alarm */
       rc = alarm(0);
       assert(rc == 0);

       return EXIT_SUCCESS;
}

