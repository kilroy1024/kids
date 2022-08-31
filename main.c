#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <sys/time.h>

#define TIMES 10
#define MULTIPLICAND_START  (3)
#define MULTIPLICAND_END    (9)
#define STAT         0
#define REL          1
#define TIMEOUT_VAL  2
#define PASS 19
#define TIME_OUT 17
#define CHECK_TIME 10

int tbl[MULTIPLICAND_END - MULTIPLICAND_START + 1][9][3] = {0};

void _show_total()
{
    int i, k;

    printf("num|01|02|03|04|05|06|07|08|09|\n");
    for (i = 0; i< MULTIPLICAND_END - MULTIPLICAND_START + 1; i++) {
    	printf("  %d|", i+MULTIPLICAND_START);
    	for (k = 0; k < 9; k++) {
	    if (tbl[i][k][REL] == PASS)
    		printf("%02s|", "--");
	    else
    		printf("%02d|", tbl[i][k][REL]);
	}
	printf("\n");
    }
}

void _show_timeout()
{
    int i, k;
    for (i = 0; i< MULTIPLICAND_END - MULTIPLICAND_START + 1; i++) {
    	for (k = 0; k < 9; k++) {
//	    if (tbl[i][k][TIMEOUT_VAL] > CHECK_TIME || tbl[i][k][REL] != PASS)
	    	printf("=> %02d x %02d time spend: %d, result = %s\n", 
		i + MULTIPLICAND_START, k+1, tbl[i][k][TIMEOUT_VAL], tbl[i][k][REL] == PASS?"pass":"fail");
	}
    }
    return ;
}

void main()
{
    int multiplicand, multiplier; 
    int i, j, k, ans;
    int pass_cnt=0;
    int times = (MULTIPLICAND_END-MULTIPLICAND_START+1)*9;
    char term;
    struct  timeval    t_start, t_end;

    srand(time(0));
    for (k=0; k<times; k++)
    {
        printf("*************************************\n");

        //b=(rand()%9)+1;
        //a=(rand()%9)+1;
	while (1) {
            multiplicand = (rand() % (MULTIPLICAND_END - MULTIPLICAND_START + 1)) + MULTIPLICAND_START;
            multiplier = (rand() % 9) + 1;

	    if (!tbl[multiplicand - MULTIPLICAND_START][multiplier-1][STAT]) {
	        tbl[multiplicand - MULTIPLICAND_START][multiplier-1][STAT] = 1;
	    	break;
	    } else { 
	    	continue;
	    }
	}
        printf("%d x %d = ? \n", multiplicand, multiplier);

	for (i=1; i<=multiplier; i++) {
	    for (j=0; j<multiplicand; j++) {
		printf("O");
	    }
	    printf("\n");
	    if (i == 2 | i == 4 | i == 8)
	        printf("\n");
	}

        gettimeofday(&t_start, NULL);
	//scanf("%d", &ans);
       char *end;
       char buf[10];
       int t_diff = 0;

        do {
            if (!fgets(buf, sizeof buf, stdin))
                break;

            buf[strlen(buf) - 1] = 0;

            ans = strtol(buf, &end, 10);
        } while (end != buf + strlen(buf));

        gettimeofday(&t_end, NULL);

	t_diff = t_end.tv_sec - t_start.tv_sec;
//	printf("spend time: %d\n", t_diff);
	
	if (ans == multiplicand * multiplier) { 
	    pass_cnt += 1;
	    tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][REL] = PASS;
	} else {
	    tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][REL] = ans;
	}
	   
	tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][TIMEOUT_VAL] = t_diff;
        printf("*************************************\n");
    }
	
    if (pass_cnt == times) {
        printf("*************************************\n");
        printf("*************************************\n");
	printf("ALL PASS\n");
        printf("*************************************\n");
        printf("*************************************\n");
    }

    printf("--------------------------------------\n");
    printf("pass %d \n", pass_cnt);
    printf("fail %d \n", times - pass_cnt);
    printf("--------------------------------------\n");
    _show_total();
    printf("--------------------------------------\n");
    _show_timeout();
    printf("--------------------------------------\n");
    printf("\n");
    printf("--------------------------------------\n");
}
