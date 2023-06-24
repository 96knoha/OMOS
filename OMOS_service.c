#include "omos.h"

/**
 * OMOSメイン関数(スレッド関数)
 * [ARGUMENT]
 *      __arg   : スレッド引数構造体(ThreadParameter型)
 * [RETURN]
 *      NONE
 */
void *omos_service(void *__arg){
    ThreadParameter *threadParam = (ThreadParameter *)__arg;
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE];
    int recvLen, sendLen;
    int perm_1, perm_2, perm_3;
    pthread_t selfId;
    int cnt, auth, kitchen_y_n;
    int table_num;
    int reg[2];

    recvLen = flag = 0;
    i = 0;

    table_num = kitchen_y_n = 0;

    selfId = pthread_self();
    printf("[C_THREAD %ld] OMOS SERVICE START (%d)\n", selfId, threadParam->soc);

    while(1){
        auth = service_user(threadParam->soc);              //ユーザー認証
        if(table_num != 0){
            service_table(threadParam->soc, auth);          //卓の処理
        }else if(kitchen_y_n != 0){
            service_kitchen(threadParam->soc, auth);        //キッチンの処理
        }else if(auth == AGUEST){
            service_guest(threadParam->soc);                //ゲストの処理
        }else{
            service_employee(threadParam->soc, auth, reg);  //店員の処理
            kitchen_y_n = reg[0];
            table_num = reg[1];
        }
    }

    printf("[C_THREAD %ld] OMOS SERVICE END (%d)\n\n", selfId, threadParam->soc);
    PQfinish(threadParam->con);
    close(threadParam->soc);
    free(threadParam);
}
