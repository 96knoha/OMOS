#include "omos.h"

int storageCheck(pthread_t selfId, PGconn *con, int soc, char *recvBuf, char *sendBuf, int *u_info){
    int recvLen, sendLen; //送受信データ長
    PGresult *res; //PGresult型の変数resを宣言
    int u_id, u_auth, u_store, o_id, o_amount, OD1, OD2, check, s_id, m_id; //u_系の変数はauthの中身チェック用、o_系統の変数は発注情報、OD1はフード・ドリンクの商品ID、OD2はフード・ドリンクの発注数、checkは発注票作成の際、動作続行のチェック用, s_idは店舗IDチェック用、mcheckはメニューIDチェック用

    u_id = u_info[0]; //ユーザID
    u_auth = u_info[1]; //ユーザの持つ権限
    u_store = u_info[2]; //ユーザの所属

    if (u_auth == ACLERK){ //店員
        //現在の在庫です。と表示
        sprintf(sendBuf, "現在の在庫です。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        // 在庫一斉表示
        //テーブル名：menu_storage_tからstore_idがu_storeのものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d", u_store); //SQL文を作成
        res = PQexec(con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        }
        PQclear(res); //resのメモリを解放
        //在庫切れが近い商品です。と表示
        sprintf(sendBuf, "在庫切れが近い商品です（発注済を除く）。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        // テーブル名：menu_storage_tからstore_idがu_storeと一致し、かつstorageの値がmin_storageよりも小さいものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するかつテーブル名：menu_storage_tのstorage_flagが0ものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d AND menu_storage_t.storage < menu_storage_t.min_storage AND menu_storage_t.storage_flag = 0", u_store); //SQL文を作成
        res = PQexec(con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        }
        PQclear(res); //resのメモリを解放
    }else if (u_auth == AMGR){ //店長
        // 在庫一斉表示
        //テーブル名：menu_storage_tからstore_idがu_storeのものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d", u_store); //SQL文を作成
        res = PQexec(con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        }
        PQclear(res); //メモリ解放
        //在庫切れが近い商品です。と表示
        sprintf(sendBuf, "在庫切れが近い商品です（発注済を除く）。%s%s", ENTER, DATA_END);
        sendLen = strlen(sendBuf);
        send(soc, sendBuf, sendLen, 0);
        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        // テーブル名：menu_storage_tからstore_idがu_storeと一致し、かつstorageの値がmin_storageよりも小さいものを抽出し、menu_id, storageを表示。また、menu_idとrecipe_tのmenu_idが一致するかつテーブル名：menu_storage_tのstorage_flagが0ものを抽出し、menu_nameを表示
        sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name, menu_storage_t.storage FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d AND menu_storage_t.storage < menu_storage_t.min_storage AND menu_storage_t.storage_flag = 0", u_store); //SQL文を作成
        res = PQexec(con, sendBuf); //送信データを実行
        // 実行結果を表示
        for (int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s %s %s %s%s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
        }
        PQclear(res); //resのメモリを解放
        while (1){
            sprintf(sendBuf, "発注しますか？ (y/n) %s%s", ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            if (strcmp(recvBuf, "y") == 0){
                sprintf(sendBuf, "フードとドリンク、どちらを発注しますか？(food/drink)%s%s", ENTER, DATA_END); //送信データ作成
                sendLen = strlen(sendBuf); //送信データ長
                send(soc, sendBuf, sendLen, 0); //送信
                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                if (strcmp(recvBuf, "food") == 0){
                    while (1){
                        check = 0;
                        sprintf(sendBuf, "どのフードを注文しますか？商品ID（4桁）を打ち込んでください。（例：0001） %s操作を終了したい場合は end と入力してください．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf);//送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0';//受信データにNULLを追加
                        if (strcmp(recvBuf, "end") == 0){
                         break;
                        }else{
                            //4文字以外の場合はエラーを返す
                            if (strlen(recvBuf) != 4){
                                sprintf(sendBuf, "商品IDは4桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            //数字以外の場合はエラーを返す
                            if (strlen(recvBuf) == 4 && !isdigit(recvBuf[0]) || !isdigit(recvBuf[1]) || !isdigit(recvBuf[2]) || !isdigit(recvBuf[3])){
                                sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            if (check != 1){
                                //入力されている商品IDの値をOD1に代入
                                sscanf(recvBuf, "%d", &OD1);
                                //テーブル名：recipe_tからOD1と同じmenu_idを持つ行があるかを確認
                                sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d", OD1); //送信データ作成
                                res = PQexec(con, sendBuf); //実行
                                // もしうまくいかなければエラーを表示する
                                if (PQresultStatus(res) != PGRES_TUPLES_OK){
                                    sprintf(sendBuf, "データベースの検索に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    check = 1;
                                }
                                //resに対してPQntuplesで行数を取得し、1行もなければエラーを表示する
                                if (PQntuples(res) == 0){
                                    sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    check = 1;
                                }
                                PQclear(res); //resのメモリを解放
                                if (check != 1){
                                    sprintf(sendBuf, "何個注文しますか？3桁で入力してください。（例：001）%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    //3文字以外の場合はエラーを返す
                                    if (strlen(recvBuf) != 3){
                                        sprintf(sendBuf, "注文数は3桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    //数字以外の場合はエラーを返す
                                    if (strlen(recvBuf) == 3 && !isdigit(recvBuf[0]) || !isdigit(recvBuf[1]) || !isdigit(recvBuf[2])){
                                        sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    //入力されている個数の値をOD2に代入
                                    sscanf(recvBuf, "%d", &OD2);
                                    //テーブル名：store_order_tのstore_idにu_storeを挿入、menu_idにOD1を挿入、store_order_cntにOD2を挿入、store_order_dateに現在の日付を挿入、store_order_timeに現在の時刻を挿入             
                                    sprintf(sendBuf, "INSERT INTO store_order_t (store_id, menu_id, store_order_cnt, store_order_date, store_order_time) VALUES (%d, %d, %d, current_date, current_time)", OD1, OD2, u_store); //SQL文作成                                    
                                    res = PQexec(con, sendBuf); //実行
                                    //失敗した場合はエラーを表示する
                                    if (PQresultStatus(res) != PGRES_COMMAND_OK){
                                        sprintf(sendBuf, "注文に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    PQclear(res); //resのメモリを解放
                                    //テーブル名：menu_storage_tのstorage_flagに1を挿入
                                    sprintf(sendBuf, "UPDATE menu_storage_t SET storage_flag = 1 WHERE menu_id = %d", OD1); //SQL文作成
                                    res = PQexec(con, sendBuf); //実行
                                    PQclear(res); //resのメモリを解放
                                }
                            }
                        }
                    }
                }else if (strcmp(recvBuf, "drink") == 0){
                    while (1){
                        check = 0;
                        sprintf(sendBuf, "どのドリンクを注文しますか？商品ID（4桁）を打ち込んでください。（例：0001） %s操作を終了したい場合は end と入力してください．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf);//送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0';//受信データにNULLを追加
                        if (strcmp(recvBuf, "end") == 0){
                         break;
                        }else{
                            //4文字以外の場合はエラーを返す
                            if (strlen(recvBuf) != 4){
                                sprintf(sendBuf, "商品IDは4桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            //数字以外の場合はエラーを返す
                            if (strlen(recvBuf) == 4 && !isdigit(recvBuf[0]) || !isdigit(recvBuf[1]) || !isdigit(recvBuf[2]) || !isdigit(recvBuf[3])){
                                sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            if (check != 1){
                                //入力されている商品IDの値をOD1に代入
                                sscanf(recvBuf, "%d", &OD1);
                                //テーブル名：recipe_tからOD1と同じmenu_idを持つ行があるかを確認
                                sprintf(sendBuf, "SELECT menu_id FROM recipe_t WHERE menu_id = %d", OD1); //送信データ作成
                                res = PQexec(con, sendBuf); //実行
                                // もしうまくいかなければエラーを表示する
                                if (PQresultStatus(res) != PGRES_TUPLES_OK){
                                    sprintf(sendBuf, "データベースの検索に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    check = 1;
                                }
                                //resに対してPQntuplesで行数を取得し、1行もなければエラーを表示する
                                if (PQntuples(res) == 0){
                                    sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    check = 1;
                                }
                                PQclear(res); //resのメモリを解放
                                if (check != 1){
                                    sprintf(sendBuf, "何個注文しますか？3桁で入力してください。（例：001）%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                    //3文字以外の場合はエラーを返す
                                    if (strlen(recvBuf) != 3){
                                        sprintf(sendBuf, "注文数は3桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    //数字以外の場合はエラーを返す
                                    if (strlen(recvBuf) == 3 && !isdigit(recvBuf[0]) || !isdigit(recvBuf[1]) || !isdigit(recvBuf[2])){
                                        sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    //入力されている個数の値をOD2に代入
                                    sscanf(recvBuf, "%d", &OD2);
                                    //テーブル名：store_order_tのstore_idにu_storeを挿入、menu_idにOD1を挿入、store_order_cntにOD2を挿入、store_order_dateに現在の日付を挿入、store_order_timeに現在の時刻を挿入             
                                    sprintf(sendBuf, "INSERT INTO store_order_t (store_id, menu_id, store_order_cnt, store_order_date, store_order_time) VALUES (%d, %d, %d, current_date, current_time)", OD1, OD2, u_store); //SQL文作成                                    
                                    res = PQexec(con, sendBuf); //実行
                                    //失敗した場合はエラーを表示する
                                    if (PQresultStatus(res) != PGRES_COMMAND_OK){
                                        sprintf(sendBuf, "注文に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                        check = 1;
                                    }
                                    PQclear(res); //resのメモリを解放
                                    //テーブル名：menu_storage_tのstorage_flagに1を挿入
                                    sprintf(sendBuf, "UPDATE menu_storage_t SET storage_flag = 1 WHERE menu_id = %d", OD1); //SQL文作成
                                    res = PQexec(con, sendBuf); //実行
                                    PQclear(res); //resのメモリを解放
                                }
                            }
                        }
                    }
                }else{
                    // 入力されている文字が不正であることを伝える。
                    sprintf(sendBuf, "入力された値が不正です。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                }
            }else if (strcmp(recvBuf, "n") == 0){
                break;
            }
        }
    }else if (u_auth == ACOR){ //COR
        while (1){
            check = 0; 
            // どの店舗IDを選ぶかを確認する。
            sprintf(sendBuf, "どの店舗IDの在庫を確認しますか？3桁で入力してください。（例：001）%s操作を終了したい場合は exit と入力してください．%s%s", ENTER, ENTER, DATA_END); //送信データ作成
            sendLen = strlen(sendBuf); //送信データ長
            send(soc, sendBuf, sendLen, 0); //送信
            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
            if (strcmp(recvBuf, "exit") == 0){
                break;
            }else{
                // recvBufの中身を確認
                if (strlen(recvBuf) != 3){ //3桁でなければエラーを表示する。
                    sprintf(sendBuf, "店舗IDは3桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                }else if (!isdigit(recvBuf[0]) || !isdigit(recvBuf[1])){ //数字以外の文字が入っていたら、エラーを表示する。
                    sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                    sendLen = strlen(sendBuf); //送信データ長
                    send(soc, sendBuf, sendLen, 0); //送信
                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                }else{
                    //s_idに入力された値を代入
                    sscanf(recvBuf, "%d", &s_id);
                    // テーブル名：menu_storage_tからstore_idがs_idと同じものを取得して表示
                    sprintf(sendBuf, "SELECT * FROM menu_storage_t WHERE store_id = %d", s_id); //SQL文作成
                    res = PQexec(con, sendBuf); //実行
                    // もしうまくいかなければエラーを表示する
                    if (PQresultStatus(res) != PGRES_TUPLES_OK){
                        sprintf(sendBuf, "データベースの検索に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        check = 1;
                    }
                    //resに対してPQntuplesで行数を取得し、1行もなければエラーを表示する
                    if (PQntuples(res) == 0){
                        sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        check = 1;
                    }
                    PQclear(res); //resのメモリを解放
                    if(check != 1){
                        // 閲覧したい商品の商品IDを入力させる。
                        sprintf(sendBuf, "閲覧したい商品の商品IDを4桁で入力してください（例：0001）。%s%s", ENTER, DATA_END); //送信データ作成
                        sendLen = strlen(sendBuf); //送信データ長
                        send(soc, sendBuf, sendLen, 0); //送信
                        recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                        recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        if (strlen(recvBuf) != 4){ //4桁でなければエラーを表示する。
                            sprintf(sendBuf, "商品IDは4桁で入力してください。%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        }else if (!isdigit(recvBuf[0]) || !isdigit(recvBuf[1]) || !isdigit(recvBuf[2]) || !isdigit(recvBuf[3])){
                            //数字以外の文字が入っていたら、エラーを表示する。
                            sprintf(sendBuf, "数字以外の文字が入力されています。%s%s", ENTER, DATA_END); //送信データ作成
                            sendLen = strlen(sendBuf); //送信データ長
                            send(soc, sendBuf, sendLen, 0); //送信
                            recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                            recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                        }else{
                            //m_idに入力された値を代入
                            sscanf(recvBuf, "%d", &m_id);
                            //テーブル名：menu_storage_tからstore_idがs_idと同じもの、かつmenu_idがm_idと同じものを取得して表示。また、テーブル名：recipe_tからmenu_idがm_idと同じもののmenu_nameを取得して表示。
                            sprintf(sendBuf, "SELECT menu_storage_t.store_id, menu_storage_t.menu_id, menu_storage_t.storage, recipe_t.menu_name FROM menu_storage_t INNER JOIN recipe_t ON menu_storage_t.menu_id = recipe_t.menu_id WHERE menu_storage_t.store_id = %d AND menu_storage_t.menu_id = %d", s_id, m_id); //SQL文作成
                            res = PQexec(con, sendBuf); //実行
                            // もしうまくいかなければエラーを表示する
                            if (PQresultStatus(res) != PGRES_TUPLES_OK){
                                sprintf(sendBuf, "データベースの検索に失敗しました。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            //resに対してPQntuplesで行数を取得し、1行もなければエラーを表示する
                            if (PQntuples(res) == 0){
                                sprintf(sendBuf, "選択した商品IDに商品が登録されていません。%s%s", ENTER, DATA_END); //送信データ作成
                                sendLen = strlen(sendBuf); //送信データ長
                                send(soc, sendBuf, sendLen, 0); //送信
                                recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                check = 1;
                            }
                            if (check != 1){
                                //実行結果を表示
                                for (int i = 0; i < PQntuples(res); i++){
                                    for (int j = 0; j < PQnfields(res); j++){
                                        sprintf(sendBuf, "%s ", PQgetvalue(res, i, j)); //送信データ作成
                                        sendLen = strlen(sendBuf); //送信データ長
                                        send(soc, sendBuf, sendLen, 0); //送信
                                    }
                                    sprintf(sendBuf, "%s%s", ENTER, DATA_END); //送信データ作成
                                    sendLen = strlen(sendBuf); //送信データ長
                                    send(soc, sendBuf, sendLen, 0); //送信
                                    recvLen = recv(soc, recvBuf, BUFSIZE, 0); //受信
                                    recvBuf[recvLen-1] = '\0'; //受信データにNULLを追加
                                }
                            }
                            PQclear(res); //resのメモリを解放
                        }
                    }
                }
            }
        }
    }
    return 0;
}