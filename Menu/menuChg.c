#include "omos.h"

int menuChg(PGconn *__con, int __soc, int *__u_info){
    int recvLen, sendLen;　//送受信データ長
    int changeid, changestore, changeprice, changelevel, u_id, u_auth, u_store; //変更する商品ID, 変更を加えたい部分の店舗ID, 変更後の値段, 押しかどうか、ユーザID、ユーザの持つ権限、ユーザの所属
    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]　//送受信用バッファ
    char response, changeitem, changename, changestar;
    pthread_t selfId = pthread_self();  //スレッド
    PGresult *res;　//PGresult型の変数resを宣言

    u_id = __u_info[0];　//ユーザID
    u_auth = __u_info[1];　//ユーザの持つ権限
    u_store = __u_info[2];　//ユーザの所属

    if(u_auth == AMGR){
        sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        //テーブル名：menu_charge_tからaccount_idがu_idと一致し、かつテーブル名：push_tでlayerの値が4のもののmenu_idを取得し、テーブル名：recipe_tからそのmenu_idのmenu_nameを表示
        sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_charge_t WHERE account_id = %d) AND menu_id IN (SELECT menu_id FROM push_t WHERE layer = 4);", u_id);　//SQL文作成
        res = PQexec(__con, sendBuf); //SQL文実行
        //クライアントに実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
        }
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した値をchangeidに代入
        sscanf(recvBuf, "%d", &changeid);
        //クライアントから受信したchangeidと、menu_idが一致するものかつテーブル名：menu_storage_tのstore_idとu_storeが一致するものを探す。
        sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, u_store);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        if(PQntuples(res) == 0){　//menu_idが存在しない場合
            sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        //何を変更したいかを聞く。選択肢はname, price, level, starであることも伝える。
        sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, starです．%s", ENTER, ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した変更内容をchangeitemに代入
        sscanf(recvBuf, "%s", changeitem);
        //変更内容がnameの場合、どう変更するかを聞く。
        if(strcmp(changeitem, "name") == 0){
            sprintf(sendBuf, "どんな商品名にしますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangenameに代入
            sscanf(recvBuf, "%s", changename);
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
            sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
        }else if(strcmp(changeitem, "price") == 0){
            //現在の価格は以下の通りです。と表示
            sprintf(sendBuf, "現在の価格は以下の通りです。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、テーブル名：price_charge_tからmenu_idがchangeidのもののpriceを表示
            sprintf(sendBuf, "SELECT price FROM price_charge_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //実行結果をクライアントに送信
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s%s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
            }
            //変更内容がpriceの場合、どう変更するかを聞く。
            sprintf(sendBuf, "値段はいくらに変更しますか？%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangepriceに代入
            sscanf(recvBuf, "%s", changeprice);
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのpriceの内容をchangepriceに変更
            sprintf(sendBuf, "UPDATE recipe_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
        }else if(strcmp(changeitem, "star") == 0){
            sprintf(sendBuf, "メニュー一覧です．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //テーブル名：menu_storage_tからstore_idがu_storeと一致する商品のmenu_id, menu_nameを表示する。menu_nameはテーブル名：recipe_tからmenu_idが一致するものを表示。
            sprintf(sendBuf, "SELECT menu_storage_t.menu_id, recipe_t.menu_name FROM menu_storage_t, recipe_t WHERE menu_storage_t.menu_id = recipe_t.menu_id AND menu_storage_t.store_id = %d;", u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf , sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            }
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したchangeidと、menu_idが一致するものかつテーブル名：menu_storage_tのstore_idとu_storeが一致するものを探す。
            sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            if(PQntuples(res) == 0){　//menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
               send(__soc, sendBuf, sendLen, 0);　//送信
               return -1;
            }
            //テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_hqの値を確認する。
            sprintf(sendBuf, "SELECT push_hq FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //push_hqの値が0の場合、押しメニューにしますか？と聞く。
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_hqの値を1に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_hq = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                }
            }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangestarに代入
                sscanf(recvBuf, "%s", changestar);
                //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとu_storeが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_hqの値を0に変更
                if(strcmp(changestar, "yes") == 0){
                    sprintf(sendBuf, "UPDATE push_t SET push_hq = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, u_store);　//SQL文作成
                    res = PQexec(__con, sendBuf);　//SQL文実行
                }
            }
            
        }else{
            //打ち込まれたコマンドが使えないことを表示
            sprintf(sendBuf, "そのコマンドは使えません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        };
    }else if(__auth == AHQ){
        //情報を変更したいものがショップメニューかどうかを聞く。
        sprintf(sendBuf, "情報を変更したいものはショップメニューですか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //受信した内容をresponseに代入
        sscanf(recvBuf, "%s", response);
        if(strcmp(response, "yes") == 0){
            //情報を変更したい店舗ID（2桁）を聞く。
            sprintf(sendBuf, "情報を変更したい店舗ID（2桁）を入力してください。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //受信した値をchangestoreに代入
            sscanf(recvBuf, "%d", &changestore);
            //情報を変更したい店舗IDが存在するかどうかをテーブル名store_tから確認
            sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //テーブル名store_tから取得した値が0の場合、店舗IDが存在しないことを表示
            if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                sprintf(sendBuf, "その店舗IDは存在しません．%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
            }
            sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //情報を変更したい店舗IDが存在する場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致するかつテーブル名：push_tでlayerの値が4のもののmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
            sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d) AND menu_id IN (SELECT menu_id FROM push_t WHERE layer = 4);", changestore);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //実行したSQL文の結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
            }
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            if(PQntuples(res) == 0){　//menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                return -1;
            }
            sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level, starです．%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //受信した内容をchangeitemに代入
            sscanf(recvBuf, "%s", changeitem);
            if(strcmp(changeitem, "name") == 0){
                sprintf(sendBuf, "どんな商品名にしますか？%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangenameに代入
                sscanf(recvBuf, "%s", changename);
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changename, changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "price") == 0){
                sprintf(sendBuf, "どんな値段にしますか？%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した値をchangepriceに代入
                sscanf(recvBuf, "%d", &changeprice);
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：recipe_tのpriceの内容をchangepriceに変更
                sprintf(sendBuf, "UPDATE recipe_t SET price = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeprice, changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "level") == 0){
                //どのメニューレベルにするかを聞く。選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華であることも伝える。
                sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華です．%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した値をchangelevelに代入
                sscanf(recvBuf, "%d", &changelevel);
                //changelevelの値が0, 1, 2, 3以外の場合、エラーを返す。
                if(changelevel != 0 && changelevel != 1 && changelevel != 2 && changelevel != 3){
                    sprintf(sendBuf, "そのメニューレベルは存在しません．%s", ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    return -1;
                }
                //テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのlayerの内容をchangelevelに変更
                sprintf(sendBuf, "UPDATE push_t SET layer = %d WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changelevel, changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "star") == 0){
                //menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの値を確認する。
                sprintf(sendBuf, "SELECT push_mgr FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
                //push_mgrの値が0の場合、押しメニューにしますか？と聞く。
                if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                    sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの内容を1に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_mgr = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
                        res = PQexec(__con, sendBuf);　//SQL文実行
                    }
                //push_mgrの値が1の場合、押しメニューをやめますか？と聞く。
                }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                    sprintf(sendBuf, "押しメニューをやめますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの内容を0に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_mgr = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
                        res = PQexec(__con, sendBuf);　//SQL文実行
                    }
                }
            }
        }else if(strcmp(response, "no") == 0){
            sprintf(sendBuf, "あなたが情報変更できるメニュー一覧です．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            //テーブル名：push_tでlayerの値が0, 1, 2, 3のもののmenu_idを持つmenu_nameをテーブル名：recipe_tから取得して表示
            sprintf(sendBuf, "SELECT menu_name FROM recipe_t WHERE menu_id IN (SELECT menu_id FROM push_t WHERE layer = 0 OR layer = 1 OR layer = 2 OR layer = 3);");　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            //実行結果を表示
            for(int i = 0; i < PQntuples(res); i++){
                sprintf(sendBuf, "%s %s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
            }
            sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した値をchangeidに代入
            sscanf(recvBuf, "%d", &changeid);
            //クライアントから受信したmenu_idがテーブル名：push_tに存在するか確認
            sprintf(sendBuf, "SELECT menu_id FROM push_t WHERE menu_id = %d;", changeid);　//SQL文作成
            res = PQexec(__con, sendBuf);　//SQL文実行
            if(PQntuples(res) == 0){　//menu_idが存在しない場合
                sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                return -1;
            }
            sprintf(sendBuf, "何を変更しますか？%s 選択肢はname, price, level starです．%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            sscanf(recvBuf, "%s", changeitem);
            if(strcmp(changeitem, "name") == 0){
                sprintf(sendBuf, "どんな商品名にしますか？%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した変更内容をchangenameに代入
                sscanf(recvBuf, "%s", changename);
                //changeidと同じmenu_idを持つ、テーブル名：recipe_tのmenu_nameの内容をchangenameに変更する。
                sprintf(sendBuf, "UPDATE recipe_t SET menu_name = '%s' WHERE menu_id = %d;", changename, changeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "price") == 0){
                sprintf(sendBuf, "どんな値段にしますか？%s", ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した値をchangepriceに代入
                sscanf(recvBuf, "%d", &changeprice);
                //changeidと同じmenu_idを持つ、テーブル名：price_charge_tのpriceの内容をchangepriceに変更する。
                sprintf(sendBuf, "UPDATE price_charge_t SET price = %d WHERE menu_id = %d;", changeprice, changeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "level") == 0){
                //どのメニューレベルにするかを聞く。選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューであることも伝える。
                sprintf(sendBuf, "どのメニューレベルにしますか？%s 選択肢は0：コモンメニュー、1：ブランドメニュー洋食、2：ブランドメニュー和食、3：ブランドメニュー中華、4：ショップメニューです．%s", ENTER, ENTER);　//送信データ作成
                sendLen = strlen(sendBuf);　//送信データ長
                send(__soc, sendBuf, sendLen, 0);　//送信
                recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                //クライアントから受信した値をchangelevelに代入
                sscanf(recvBuf, "%d", &changelevel);
                //changelevelの値が0, 1, 2, 3, 4以外の場合、エラーを返す。
                if(changelevel != 0 && changelevel != 1 && changelevel != 2 && changelevel != 3 && changelevel != 4){
                    sprintf(sendBuf, "そのメニューレベルは存在しません．%s", ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    return -1;
                }
                //changeidと同じmenu_idを持つ、テーブル名：push_tのlayerの内容をchangelevelに変更する。
                sprintf(sendBuf, "UPDATE push_t SET layer = %d WHERE menu_id = %d;", changelevel, changeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }else if(strcmp(changeitem, "star") == 0){
                //changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの値を確認する。
                sprintf(sendBuf, "SELECT push_mgr FROM push_t WHERE menu_id = %d;", changeid);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
                //push_mgrの値が0の場合、押しメニューにしますか？と聞く。
                if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
                    sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの内容を1に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_mgr = 1 WHERE menu_id = %d;", changeid);　//SQL文作成
                        res = PQexec(__con, sendBuf);　//SQL文実行
                    }
                }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
                    sprintf(sendBuf, "押しメニューをやめますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
                    sendLen = strlen(sendBuf);　//送信データ長
                    send(__soc, sendBuf, sendLen, 0);　//送信
                    recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
                    recvBuf{recvLen} = '\0';　//受信データにNULLを追加
                    //クライアントから受信した変更内容をchangestarに代入
                    sscanf(recvBuf, "%s", changestar);
                    //changestarがyesの場合、changeidと同じmenu_idを持つ、テーブル名：push_tのpush_mgrの内容を0に変更
                    if(strcmp(changestar, "yes") == 0){
                        sprintf(sendBuf, "UPDATE push_t SET push_mgr = 0 WHERE menu_id = %d;", changeid);　//SQL文作成
                        res = PQexec(__con, sendBuf);　//SQL文実行
                    }
                }
            }
        }else{
            sprintf(sendBuf, "無効なコマンドです．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
    }else if(__auth == ACOR){
        sprintf(sendBuf, "選べる店舗IDです．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        //u_storeと一致するregion_idを持つ、テーブル名：region_tのstore_idを表示
        sprintf(sendBuf, "SELECT store_id FROM region_t WHERE region_id = %d;", __u_store);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "store_id:%s%s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
        }
        //どの店舗IDを選択するかを聞く
        sprintf(sendBuf, "どの店舗IDを選択しますか？（2桁）%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した店舗IDをchangestoreに代入
        sscanf(recvBuf, "%d", &changestore);
        //情報を変更したい店舗IDが存在するかどうかをテーブル名store_tから確認
        sprintf(sendBuf, "SELECT COUNT(*) FROM store_t WHERE store_id = %d;", changestore);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //テーブル名store_tから取得した値が0の場合、店舗IDが存在しないことを表示
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "その店舗IDは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        sprintf(sendBuf, "メニュー一覧です．%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        //changestoreと同じ値のstore_idとを持っているmenu_idをテーブル名：menu_storage_tから取得
        sprintf(sendBuf, "SELECT menu_id FROM menu_storage_t WHERE store_id = %d;", changestore);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //実行結果を表示
        for(int i = 0; i < PQntuples(res); i++){
            sprintf(sendBuf, "menu_id:%s%s", PQgetvalue(res, i, 0), ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
        }
        sprintf(sendBuf, "どのメニューを変更しますか？商品ID（4桁）を打ち込んでください。%s", ENTER);　//送信データ作成
        sendLen = strlen(sendBuf);　//送信データ長
        send(__soc, sendBuf, sendLen, 0);　//送信
        recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
        recvBuf{recvLen} = '\0';　//受信データにNULLを追加
        //クライアントから受信した値をchangeidに代入
        sscanf(recvBuf, "%d", &changeid);
        //changeidがchangestoreと同じ値のstore_idを持つmenu_storage_tに存在するか確認
        sprintf(sendBuf, "SELECT COUNT(*) FROM menu_storage_t WHERE menu_id = %d AND store_id = %d;", changeid, changestore);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //テーブル名menu_storage_tから取得した値が0の場合、menu_idが存在しないことを表示
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "そのメニューは存在しません．%s", ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            return -1;
        }
        //changeidと同じmenu_idを持つテーブル名：push_tのpush_corの値を取得する。このときテーブル名：menu_storage_tを見てstore_idとchangestoreが一致していなければならない。
        sprintf(sendBuf, "SELECT push_cor FROM push_t WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
        res = PQexec(__con, sendBuf);　//SQL文実行
        //push_corの値が0の場合、押しメニューにしますか？と聞く。
        if(strcmp(PQgetvalue(res, 0, 0), "0") == 0){
            sprintf(sendBuf, "押しメニューにしますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_corの値を1に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 1 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実
            }
            //push_corの値が0の場合、押しメニューをやめますか？と聞く。
        }else if(strcmp(PQgetvalue(res, 0, 0), "1") == 0){
            sprintf(sendBuf, "押しにするのを止めますか？%s yes または no%s", ENTER, ENTER);　//送信データ作成
            sendLen = strlen(sendBuf);　//送信データ長
            send(__soc, sendBuf, sendLen, 0);　//送信
            recvLen = recv(__soc, recvBuf, BUFSIZE, 0);　//受信
            recvBuf{recvLen} = '\0';　//受信データにNULLを追加
            //クライアントから受信した変更内容をchangestarに代入
            sscanf(recvBuf, "%s", changestar);
            //クライアントから受信したchangestarがyesの場合、テーブル名：menu_storage_tのstore_idとchangestoreが一致し、changeidと一致するmenu_idを持つテーブル名：push_tのpush_corの値を0に変更
            if(strcmp(changestar, "yes") == 0){
                sprintf(sendBuf, "UPDATE push_t SET push_cor = 0 WHERE menu_id = %d AND menu_id IN (SELECT menu_id FROM menu_storage_t WHERE store_id = %d);", changeid, changestore);　//SQL文作成
                res = PQexec(__con, sendBuf);　//SQL文実行
            }
        }
    }
    return 0;
}