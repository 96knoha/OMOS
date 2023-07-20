#include "omos.h"

\* お会計処理 *\
int pay(PGconn *con, int soc, int *u_info, pthread_t selfId)
{
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ
    int recvLen, sendLen;                    // 送受信データ長
    int i=0, num, money,change;
    char query[1024];

    // トランザクション開始
    PGresult *res = PQexec(con, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("config(1)\n");
        printf("BEGIN failed: %s", PQerrorMessage(con));
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
    }

    // Get the store number from u_info[2].
    int store_num = u_info[2];
    printf("store_num = %d\n", store_num);

    // お会計にてテーブル番号を入力してもらう
    sprintf(sendBuf, "テーブル番号を入力してください。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    recvLen = receive_message(soc, recvBuf, BUFSIZE);
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf);

    // テーブル番号の取得
    int tableNum = atoi(recvBuf);
    printf("tableNum = %d\n", tableNum);

    // Check if there is a store_table_t table whose store_id matches store_num and desk_num matches tableNum.
    sprintf(sendBuf, "SELECT * FROM store_table_t WHERE store_id = %d AND desk_num = %d;", store_num, tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return -1;
    }

    // テーブル番号が存在しない場合
    if (PQntuples(res) == 0)
    {
        sprintf(sendBuf, "テーブル番号が存在しません。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return -1;
    }

    // テーブル番号が存在する場合
    // order_tよりテーブル番号(tableNum = desk_num)の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return 0;
    }

    // 注文が存在しない場合
    if (PQntuples(res) == 0)
    {
        sprintf(sendBuf, "注文が存在しません。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return 0;
    }

    // 注文が存在する場合
    // order_tよりkitchen_flagが0の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 0;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s", ENTER);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s", ENTER);
        send(soc, sendBuf, sendLen, 0);
        return 0;
    }

    // kitchen_flagが0の注文が存在する場合
    if (PQntuples(res) != 0)
    {
        sprintf(sendBuf, "まだ提供していない料理がございます。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        PQclear(res);
        return 0;
    }

    // order_tよりkitchen_flagが1の注文を取得する
    sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return 0;
    }

    // kitchen_flagが1の注文が存在する場合、割り勘の有無を問う
    if (PQntuples(res) != 0)
    {
        sprintf(sendBuf, "割り勘しますか？(y/n)%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
        recvBuf[recvLen-1] = '\0'; //受信データを文字列にする

        // 割り勘する場合
        if (recvBuf[0] == 'y')
        {
            // order_tよりkitchen_flagが1の注文を取得する
            sprintf(sendBuf, "SELECT * FROM order_t WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
            res = PQexec(con, sendBuf);
            if (PQresultStatus(res) != PGRES_TUPLES_OK)
            { // SELECT失敗
                printf("SELECT failed: %s", PQerrorMessage(con));
                // ロールバック
                res = PQexec(con, "ROLLBACK");
                if (PQresultStatus(res) != PGRES_COMMAND_OK)
                {
                    printf("ROLLBACK failed: %s", PQerrorMessage(con));
                    PQclear(res);
                    sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
                    sendLen = strlen(sendBuf);
                    send(soc, sendBuf, sendLen, 0);
                }
                PQclear(res);
                sprintf(sendBuf, "error occured%s%s", ENTER,DATA_END);
                send(soc, sendBuf, sendLen, 0);
                return 0;
            }

            // 割り勘する人数を入力してもらう
            sprintf(sendBuf, "割り勘する人数を入力してください。%s%s", ENTER,DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            // 割り勘する人数の取得
            num = atoi(recvBuf);

            // 割り勘する人数が0以下の場合、もう一度入力してもらう
            while (num <= 0)
            {
                sprintf(sendBuf, "入力値が不正です。割り勘する人数を入力してください。%s%s", ENTER,DATA_END );
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
                recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
                printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

                // 割り勘する人数の取得
                num = atoi(recvBuf);
            }

            // 割り勘する人数の取得
            sprintf(sendBuf, "割り勘する人数は%d人です。%s", num, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        // 割り勘しない場合
        else if(recvBuf[0] == 'n')
        {
            int num = 1;
            sprintf(sendBuf, "割り勘しません。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        else{
            sprintf(sendBuf, "入力値が不正です。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }
    }

    // order_tよりkitchen_flagが1の注文を取得した後、order_tのmenu_idをもとにmenu_price_tからpriceを結合する
    sprintf(sendBuf, "SELECT order_t.menu_id, menu_price_t.price FROM order_t INNER JOIN menu_price_t ON order_t.menu_id = menu_price_t.menu_id WHERE desk_num = %d AND kitchen_flag = 1;", tableNum);
    res = PQexec(con, sendBuf);
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    { // SELECT失敗
        printf("SELECT failed: %s", PQerrorMessage(con));
        // ロールバック
        res = PQexec(con, "ROLLBACK");
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            printf("ROLLBACK failed: %s", PQerrorMessage(con));
            PQclear(res);
            sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
        }
        PQclear(res);
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        return 0;
    }

    // 金額を計算する
    // order_tの個数(order_cnt)とmenu_price_tの値段(price)をかけて、合計金額を計算する
    int sum;
    for (int i = 0; i < PQntuples(res); i++)
    {
        sum += atoi(PQgetvalue(res, i, 0)) * atoi(PQgetvalue(res, i, 1));
    }

    printf("sum = %d\n", sum);


    // ポイントの使用を問う
    sprintf(sendBuf, "ポイントを使用しますか？(y/n)%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示


    // Pointを使用する場合
    if (strcmp(recvBuf, "y") == 0 || strcmp(recvBuf, "Y") == 0){
        sprintf(sendBuf, "ポイントを使用します%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    // 割り勘しない場合
    else if(strcmp(recvBuf, "n") == 0 || strcmp(recvBuf, "N") == 0){
        sprintf(sendBuf, "ポイントを使用しない。%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    else{
        sprintf(sendBuf, "入力値が不正です。%s", ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    if(num == 0){
        num = 1;
    }
    printf("sum = %d\n", sum);
    // 割り勘する人数で割る
    sum = sum / num;      // 合計金額を人数で割る
    int over = sum % num; // 割り切れない場合の余りを求める

    // もしoverが0の時、
    if (over == 0)
    {
        // 合計金額を送信する
        sprintf(sendBuf, "合計金額は%d円です。%s", sum, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    // もしover(あまり)が0でない場合、
    else
    {
        // ○人は△円、□人は▲円です。と送信する
        sprintf(sendBuf, "%d人は%d円、%d人は%d円です。%s", num - over, sum, over, sum + 1, ENTER);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    }

    // お金を受け取り、おつり計算を行う。
    if (num == 1)
    {
        // お客様から頂戴した金額を入力してもらう
        sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
        recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
        printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

        // お客様から頂戴した金額を取得
        money = atoi(recvBuf);
        //If money exceeds 10000000, issue an amount error.
        if(money > 10000000){
            sprintf(sendBuf, "金額が大きすぎます。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            return -1;
        }

        // お客様から頂戴した金額が合計金額よりも少ない場合、
        else if(money < sum)
        {
            // お客様から頂戴した金額が合計金額よりも少ないことを伝える
            sprintf(sendBuf, "お客様から頂戴した金額が合計金額よりも少ないです。%s%s", ENTER,DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            return -1;
        }
        else{
            // おつりを計算する
            change = money - sum;

            // おつりを送信する
            sprintf(sendBuf, "おつりは%d円です。お会計終了です。%s", change, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }
    }

    else{
        //(num - over)回sum円の会計を行う、over回sum+1円の会計を行う
        for(i=0;i<num-over;i++){
            sprintf(sendBuf, "%d回目の会計,%d円です。%s", i+1, sum, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

            sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            money = 0;

            // お客様から頂戴した金額を取得
            money = atoi(recvBuf);

            //If money exceeds 10000000, issue an amount error.
            if(money > 10000000){
                sprintf(sendBuf, "金額が大きすぎます。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                break;
            }

            // お客様から頂戴した金額が合計金額よりも少ない場合、
            while (money < sum)
            {
                // お客様から頂戴した金額が合計金額よりも少ないことを伝える
                sprintf(sendBuf, "お客様から頂戴した金額が合計金額よりも少ないです。もう一度入力してください。%s%s", ENTER,DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信

                // お客様から頂戴した金額を取得
                money = atoi(recvBuf);
            }

            // お客様から頂戴した金額が合計金額よりも多い場合、
            if (money >= sum)
            {
                // おつりを計算する
                change = money - sum;

                // おつりを送信する
                sprintf(sendBuf, "おつりは%d円です。%s", change, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            }

            // お会計終了を伝える
            sprintf(sendBuf, "お会計終了です。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }

        for(i=0;i<over;i++){
            sprintf(sendBuf, "%d回目の会計,%d円です。%s", i+num-over+1, sum+1, ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示

            sprintf(sendBuf, "お客様から頂戴した金額を入力してください。%s%s", ENTER, DATA_END);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
            recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
            printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

            money = 0;

            // お客様から頂戴した金額を取得
            money = atoi(recvBuf);
            //If money exceeds 10000000, issue an amount error.
            if(money > 10000000){
                sprintf(sendBuf, "金額が大きすぎます。%s%s", ENTER, DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                break;
            }

            // お客様から頂戴した金額が合計金額よりも少ない場合、
            while (money < sum)
            {
                // お客様から頂戴した金額が合計金額よりも少ないことを伝える
                sprintf(sendBuf, "お客様から頂戴した金額が合計金額よりも少ないです。もう一度入力してください。%s%s", ENTER,DATA_END);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
                recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信

                // お客様から頂戴した金額を取得
                money = atoi(recvBuf);
            }

            // お客様から頂戴した金額が合計金額よりも多い場合、
            if (money >= sum)
            {
                // おつりを計算する
                change = money - sum;

                // おつりを送信する
                sprintf(sendBuf, "おつりは%d円です。%s", change, ENTER);
                sendLen = strlen(sendBuf);
                send(soc, sendBuf, sendLen, 0);
                printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
            }

            // お会計終了を伝える
            sprintf(sendBuf, "お会計終了です。%s", ENTER);
            sendLen = strlen(sendBuf);
            send(soc, sendBuf, sendLen, 0);
            printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
        }
    }

    printf("yeah\n");
    printf("%d\n", tableNum);

    // お会計終了を伝える
    sprintf(sendBuf, "ありがとうございました。評価に移ります。%s%s", ENTER, DATA_END);
    sendLen = strlen(sendBuf);
    send(soc, sendBuf, sendLen, 0);
    printf("[C_THREAD %ld] SEND=> %s\n", selfId, sendBuf); // 送信データを表示
    recvLen = receive_message(soc, recvBuf, BUFSIZE);      // 受信
    recvBuf[recvLen-1] = '\0'; //受信データを文字列にする
    printf("[C_THREAD %ld] RECV=> %s\n", selfId, recvBuf); // 受信データを表示

    // order_tから該当するテーブルのデータを移行する
    sprintf(query, "INSERT INTO summary_t SELECT store_id, menu_id, order_cnt, order_date, order_time, user_id FROM order_t WHERE desk_num = %d;", tableNum);
    printf("%s\n", query);
    res = PQexec(con, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("%s",PQresultErrorMessage(res));
        printf("error\n");
        PQclear(res);
        res = PQexec(con, "ROLLBACK");
        PQclear(res);
        return 1;
    }
    PQclear(res);

    printf("order_tからsummary_tへの移行完了\n");

    //order_tから該当する店舗番号、テーブルのデータを削除する
    //sprintf(query, "DELETE FROM order_t WHERE store_id = %d AND desk_num = %d;", store_num, tableNum);
    //res = PQexec(con, query);
    //if (PQresultStatus(res) != PGRES_COMMAND_OK){
    //    PQclear(res);
    //    res = PQexec(con, "ROLLBACK");
    //    PQclear(res);
    //    return 1;
    //}
    //PQclear(res);

    printf("order_tからの削除完了\n");

    // お客様の評価を行う,評価を行う関数を呼び出す
    int user_id = u_info[0];
    int flag = 0;
    printf("user_id = %d\n", user_id);

    // flagが1の場合、エラーが発生したことを伝える
    if (flag == 1)
    {
        sprintf(sendBuf, "error occured%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
    }
    // flagが0の場合、正常に評価が行われたことを伝える
    else
    {
        sprintf(sendBuf, "評価ありがとうございました。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
    }

    // トランザクションの終了
    res = PQexec(con, "COMMIT");
    PQclear(res);

    return 0;
}