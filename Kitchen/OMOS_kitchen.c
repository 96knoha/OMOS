{"payload":{"allShortcutsEnabled":true,"fileTree":{"":{"items":[{"name":"Correct","path":"Correct","contentType":"directory"},{"name":"Demand","path":"Demand","contentType":"directory"},{"name":"Evaluation","path":"Evaluation","contentType":"directory"},{"name":"Kitchen","path":"Kitchen","contentType":"directory"},{"name":"Menu","path":"Menu","contentType":"directory"},{"name":"Order","path":"Order","contentType":"directory"},{"name":"Pay","path":"Pay","contentType":"directory"},{"name":"Reserve_customer","path":"Reserve_customer","contentType":"directory"},{"name":"Reserve_store","path":"Reserve_store","contentType":"directory"},{"name":"Stock","path":"Stock","contentType":"directory"},{"name":"Table","path":"Table","contentType":"directory"},{"name":"User","path":"User","contentType":"directory"},{"name":"base","path":"base","contentType":"directory"},{"name":"comm_test","path":"comm_test","contentType":"directory"},{"name":"http_server","path":"http_server","contentType":"directory"},{"name":"server","path":"server","contentType":"directory"},{"name":".gitignore","path":".gitignore","contentType":"file"},{"name":"OMOS_kitchen.c","path":"OMOS_kitchen.c","contentType":"file"},{"name":"OMOS_kitchenFlag.c","path":"OMOS_kitchenFlag.c","contentType":"file"},{"name":"OMOS_service.c","path":"OMOS_service.c","contentType":"file"},{"name":"comm_test.zip","path":"comm_test.zip","contentType":"file"},{"name":"error.c","path":"error.c","contentType":"file"}],"totalCount":22}},"fileTreeProcessingTime":4.321639,"foldersToFetch":[],"reducedMotionEnabled":"system","repo":{"id":654632070,"defaultBranch":"develop","name":"OMOS","ownerLogin":"remon-nomer66","currentUserCanPush":true,"isFork":false,"isEmpty":false,"createdAt":"2023-06-16T23:59:38.000+09:00","ownerAvatar":"https://avatars.githubusercontent.com/u/83967946?v=4","public":false,"private":true,"isOrgOwned":false},"refInfo":{"name":"develop","listCacheKey":"v0:1688566028.0","canEdit":true,"refType":"branch","currentOid":"64b696d826a17672ab28a9f9b2905867e04f8448"},"path":"OMOS_kitchen.c","currentUser":{"id":83967946,"login":"remon-nomer66","userEmail":"c.lemon1366@gmail.com"},"blob":{"rawBlob":"#include \"omos.h\"\n\nint kitchen(PGconn *__con, int __soc, int __tableNum)\n{\n    char recvBuf[BUFSIZE], sendBuf[BUFSIZE]; // 送受信用バッファ\n    int recvLen, sendLen;                    // 送受信データ長\n    pthread_t selfId = pthread_self();       // 自スレッドID\n    char comm[BUFSIZE];                      // SQLコマンド\n    int resultRows;\n    int i;\n    char sql[BUFSIZE], buf[BUFSIZE];\n    // recipe = recipe_infomation(__soc, menu_id, selfid);\n    while (1)\n    {\n        sprintf(sendbuf, \"注文されたメニューを表示したい場合はENTER、終了したい場合は\\\"END\\\"を押してください。%s\", ENTER); // 注文されたメニューを表示\n        sendLen = strlen(sendBuf);                                                                                         // 送信データ長\n        send(__soc, sendBuf, sendLen, 0);                                                                                  // 送信\n        printf(\"[C_THREAD %ld] SEND=> %s\\n\", selfId, sendBuf);                                                             // 送信データ表示\n\n        recvLen = receive_message(__soc, recvBuf, BUFSIZE); // 受信\n        if (recvLen != 0)\n        {\n            printf(\"[C_THREAD %ld] RECV=> %s\\n\", selfId, recvBuf); // 受信データ表示\n            sscanf(recvBuf, \"%s\", comm);                           // SQLコマンドを取得\n            if (strcmp(comm, ENTER) == 0)\n            {\n                sendBuf[0] = '\\0'; // 送信バッファ初期化                \n                sprintf(sql, \"SELECT food_id, order_cnt, order_flag, order_time FROM kconfirm ORDER BY order_time ASC\");\n                res = PQexec(__con, sql);\n                if (PQresultStatus(res) != PGRES_TUPLES_OK)\n                {\n                    printf(\"%s\", PQresultErrorMessage(res));\n                    return -1;\n                }\n                else\n                {\n                    resultRows = PQntuples(res);\n                    if (resultRows == 0)\n                    {\n                        sprintf(sendBuf, \"注文されたメニューはありません%s\", ENTER);\n                        return 0; // 注文されたメニューを表示\n                    }\n                    else\n                    {\n                        sprintf(sendBuf, \"商品ID、注文数、注文状況、注文時間%s\", ENTER); // 注文されたメニューを表示\n                        for (i = 0; i < resultRows; i++)\n                        {\n                            sprintf(buf, \"%s %s %s %s %s\", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3), ENTER);\n                            strcat(sendBuf, buf);\n                        }\n                    }\n                    sendLen = strlen(sendBuf);                             // 送信データ長\n                    send(__soc, sendBuf, sendLen, 0);                      // 送信\n                    printf(\"[C_THREAD %ld] SEND=> %s\\n\", selfId, sendBuf); // 送信データ表示\n                    // 注文されたメニューを表示\n                }\n            }\n            else if (strcmp(comm, END) == 0)\n            {\n                sprintf(sql, \"SELECT * FROM table_info WHERE t_flug = 1\");\n                res = PQexec(__con, sql);\n                if (PQresultStatus(res) != PGRES_TUPLES_OK)\n                {\n                    printf(\"%s\", PQresultErrorMessage(res));\n                    return -1;\n                }\n                else\n                {\n                    resultRows = PQntuples(res);\n                    if (resultRows >= 0)\n                    {\n                        sprintf(sendBuf, \"キッチンは削除できません%s\", ENTER);\n                        sendLen = strlen(sendBuf);                             // 送信データ長\n                        send(__soc, sendBuf, sendLen, 0);                      // 送信\n                        printf(\"[C_THREAD %ld] SEND=> %s\\n\", selfId, sendBuf); // 送信データ表示\n                    }\n                    else\n                    {\n                        sprintf(sendBuf, \"キッチンを削除します%s\", ENTER);\n                        sendLen = strlen(sendBuf);                             // 送信データ長\n                        send(__soc, sendBuf, sendLen, 0);                      // 送信\n                        printf(\"[C_THREAD %ld] SEND=> %s\\n\", selfId, sendBuf); // 送信データ表示\n                    }\n                }\n            }\n        }\n    }\n}","colorizedLines":null,"stylingDirectives":[[{"start":0,"end":8,"cssClass":"pl-k"},{"start":9,"end":17,"cssClass":"pl-s"}],[],[{"start":0,"end":3,"cssClass":"pl-smi"},{"start":4,"end":11,"cssClass":"pl-en"},{"start":12,"end":18,"cssClass":"pl-smi"},{"start":19,"end":20,"cssClass":"pl-c1"},{"start":20,"end":25,"cssClass":"pl-s1"},{"start":27,"end":30,"cssClass":"pl-smi"},{"start":31,"end":36,"cssClass":"pl-s1"},{"start":38,"end":41,"cssClass":"pl-smi"},{"start":42,"end":52,"cssClass":"pl-s1"}],[],[{"start":4,"end":8,"cssClass":"pl-smi"},{"start":9,"end":16,"cssClass":"pl-s1"},{"start":17,"end":24,"cssClass":"pl-c1"},{"start":27,"end":34,"cssClass":"pl-s1"},{"start":35,"end":42,"cssClass":"pl-c1"},{"start":45,"end":56,"cssClass":"pl-c"}],[{"start":4,"end":7,"cssClass":"pl-smi"},{"start":8,"end":15,"cssClass":"pl-s1"},{"start":17,"end":24,"cssClass":"pl-s1"},{"start":45,"end":55,"cssClass":"pl-c"}],[{"start":4,"end":13,"cssClass":"pl-smi"},{"start":14,"end":20,"cssClass":"pl-s1"},{"start":21,"end":22,"cssClass":"pl-c1"},{"start":23,"end":35,"cssClass":"pl-en"},{"start":45,"end":55,"cssClass":"pl-c"}],[{"start":4,"end":8,"cssClass":"pl-smi"},{"start":9,"end":13,"cssClass":"pl-s1"},{"start":14,"end":21,"cssClass":"pl-c1"},{"start":45,"end":55,"cssClass":"pl-c"}],[{"start":4,"end":7,"cssClass":"pl-smi"},{"start":8,"end":18,"cssClass":"pl-s1"}],[{"start":4,"end":7,"cssClass":"pl-smi"},{"start":8,"end":9,"cssClass":"pl-s1"}],[{"start":4,"end":8,"cssClass":"pl-smi"},{"start":9,"end":12,"cssClass":"pl-s1"},{"start":13,"end":20,"cssClass":"pl-c1"},{"start":23,"end":26,"cssClass":"pl-s1"},{"start":27,"end":34,"cssClass":"pl-c1"}],[{"start":4,"end":58,"cssClass":"pl-c"}],[{"start":4,"end":9,"cssClass":"pl-k"},{"start":11,"end":12,"cssClass":"pl-c1"}],[],[{"start":8,"end":15,"cssClass":"pl-en"},{"start":16,"end":23,"cssClass":"pl-s1"},{"start":25,"end":77,"cssClass":"pl-s"},{"start":79,"end":84,"cssClass":"pl-c1"},{"start":87,"end":102,"cssClass":"pl-c"}],[{"start":8,"end":15,"cssClass":"pl-s1"},{"start":16,"end":17,"cssClass":"pl-c1"},{"start":18,"end":24,"cssClass":"pl-en"},{"start":25,"end":32,"cssClass":"pl-s1"},{"start":123,"end":132,"cssClass":"pl-c"}],[{"start":8,"end":12,"cssClass":"pl-en"},{"start":13,"end":18,"cssClass":"pl-s1"},{"start":20,"end":27,"cssClass":"pl-s1"},{"start":29,"end":36,"cssClass":"pl-s1"},{"start":38,"end":39,"cssClass":"pl-c1"},{"start":123,"end":128,"cssClass":"pl-c"}],[{"start":8,"end":14,"cssClass":"pl-en"},{"start":15,"end":43,"cssClass":"pl-s"},{"start":45,"end":51,"cssClass":"pl-s1"},{"start":53,"end":60,"cssClass":"pl-s1"},{"start":123,"end":133,"cssClass":"pl-c"}],[],[{"start":8,"end":15,"cssClass":"pl-s1"},{"start":16,"end":17,"cssClass":"pl-c1"},{"start":18,"end":33,"cssClass":"pl-en"},{"start":34,"end":39,"cssClass":"pl-s1"},{"start":41,"end":48,"cssClass":"pl-s1"},{"start":50,"end":57,"cssClass":"pl-c1"},{"start":60,"end":65,"cssClass":"pl-c"}],[{"start":8,"end":10,"cssClass":"pl-k"},{"start":12,"end":19,"cssClass":"pl-s1"},{"start":20,"end":22,"cssClass":"pl-c1"},{"start":23,"end":24,"cssClass":"pl-c1"}],[],[{"start":12,"end":18,"cssClass":"pl-en"},{"start":19,"end":47,"cssClass":"pl-s"},{"start":49,"end":55,"cssClass":"pl-s1"},{"start":57,"end":64,"cssClass":"pl-s1"},{"start":67,"end":77,"cssClass":"pl-c"}],[{"start":12,"end":18,"cssClass":"pl-en"},{"start":19,"end":26,"cssClass":"pl-s1"},{"start":28,"end":32,"cssClass":"pl-s"},{"start":34,"end":38,"cssClass":"pl-s1"},{"start":67,"end":80,"cssClass":"pl-c"}],[{"start":12,"end":14,"cssClass":"pl-k"},{"start":16,"end":22,"cssClass":"pl-en"},{"start":23,"end":27,"cssClass":"pl-s1"},{"start":29,"end":34,"cssClass":"pl-c1"},{"start":36,"end":38,"cssClass":"pl-c1"},{"start":39,"end":40,"cssClass":"pl-c1"}],[],[{"start":16,"end":23,"cssClass":"pl-s1"},{"start":24,"end":25,"cssClass":"pl-c1"},{"start":27,"end":28,"cssClass":"pl-c1"},{"start":29,"end":33,"cssClass":"pl-c1"},{"start":35,"end":63,"cssClass":"pl-c"}],[{"start":16,"end":23,"cssClass":"pl-en"},{"start":24,"end":27,"cssClass":"pl-s1"},{"start":29,"end":118,"cssClass":"pl-s"}],[{"start":16,"end":19,"cssClass":"pl-s1"},{"start":20,"end":21,"cssClass":"pl-c1"},{"start":22,"end":28,"cssClass":"pl-en"},{"start":29,"end":34,"cssClass":"pl-s1"},{"start":36,"end":39,"cssClass":"pl-s1"}],[{"start":16,"end":18,"cssClass":"pl-k"},{"start":20,"end":34,"cssClass":"pl-en"},{"start":35,"end":38,"cssClass":"pl-s1"},{"start":40,"end":42,"cssClass":"pl-c1"},{"start":43,"end":58,"cssClass":"pl-c1"}],[],[{"start":20,"end":26,"cssClass":"pl-en"},{"start":27,"end":31,"cssClass":"pl-s"},{"start":33,"end":53,"cssClass":"pl-en"},{"start":54,"end":57,"cssClass":"pl-s1"}],[{"start":20,"end":26,"cssClass":"pl-k"},{"start":27,"end":29,"cssClass":"pl-c1"}],[],[{"start":16,"end":20,"cssClass":"pl-k"}],[],[{"start":20,"end":30,"cssClass":"pl-s1"},{"start":31,"end":32,"cssClass":"pl-c1"},{"start":33,"end":42,"cssClass":"pl-en"},{"start":43,"end":46,"cssClass":"pl-s1"}],[{"start":20,"end":22,"cssClass":"pl-k"},{"start":24,"end":34,"cssClass":"pl-s1"},{"start":35,"end":37,"cssClass":"pl-c1"},{"start":38,"end":39,"cssClass":"pl-c1"}],[],[{"start":24,"end":31,"cssClass":"pl-en"},{"start":32,"end":39,"cssClass":"pl-s1"},{"start":41,"end":60,"cssClass":"pl-s"},{"start":62,"end":67,"cssClass":"pl-c1"}],[{"start":24,"end":30,"cssClass":"pl-k"},{"start":31,"end":32,"cssClass":"pl-c1"},{"start":34,"end":49,"cssClass":"pl-c"}],[],[{"start":20,"end":24,"cssClass":"pl-k"}],[],[{"start":24,"end":31,"cssClass":"pl-en"},{"start":32,"end":39,"cssClass":"pl-s1"},{"start":41,"end":63,"cssClass":"pl-s"},{"start":65,"end":70,"cssClass":"pl-c1"},{"start":73,"end":88,"cssClass":"pl-c"}],[{"start":24,"end":27,"cssClass":"pl-k"},{"start":29,"end":30,"cssClass":"pl-s1"},{"start":31,"end":32,"cssClass":"pl-c1"},{"start":33,"end":34,"cssClass":"pl-c1"},{"start":36,"end":37,"cssClass":"pl-s1"},{"start":38,"end":39,"cssClass":"pl-c1"},{"start":40,"end":50,"cssClass":"pl-s1"},{"start":52,"end":53,"cssClass":"pl-s1"},{"start":53,"end":55,"cssClass":"pl-c1"}],[],[{"start":28,"end":35,"cssClass":"pl-en"},{"start":36,"end":39,"cssClass":"pl-s1"},{"start":41,"end":57,"cssClass":"pl-s"},{"start":59,"end":69,"cssClass":"pl-en"},{"start":70,"end":73,"cssClass":"pl-s1"},{"start":75,"end":76,"cssClass":"pl-s1"},{"start":78,"end":79,"cssClass":"pl-c1"},{"start":82,"end":92,"cssClass":"pl-en"},{"start":93,"end":96,"cssClass":"pl-s1"},{"start":98,"end":99,"cssClass":"pl-s1"},{"start":101,"end":102,"cssClass":"pl-c1"},{"start":105,"end":115,"cssClass":"pl-en"},{"start":116,"end":119,"cssClass":"pl-s1"},{"start":121,"end":122,"cssClass":"pl-s1"},{"start":124,"end":125,"cssClass":"pl-c1"},{"start":128,"end":138,"cssClass":"pl-en"},{"start":139,"end":142,"cssClass":"pl-s1"},{"start":144,"end":145,"cssClass":"pl-s1"},{"start":147,"end":148,"cssClass":"pl-c1"},{"start":151,"end":156,"cssClass":"pl-c1"}],[{"start":28,"end":34,"cssClass":"pl-en"},{"start":35,"end":42,"cssClass":"pl-s1"},{"start":44,"end":47,"cssClass":"pl-s1"}],[],[],[{"start":20,"end":27,"cssClass":"pl-s1"},{"start":28,"end":29,"cssClass":"pl-c1"},{"start":30,"end":36,"cssClass":"pl-en"},{"start":37,"end":44,"cssClass":"pl-s1"},{"start":75,"end":84,"cssClass":"pl-c"}],[{"start":20,"end":24,"cssClass":"pl-en"},{"start":25,"end":30,"cssClass":"pl-s1"},{"start":32,"end":39,"cssClass":"pl-s1"},{"start":41,"end":48,"cssClass":"pl-s1"},{"start":50,"end":51,"cssClass":"pl-c1"},{"start":75,"end":80,"cssClass":"pl-c"}],[{"start":20,"end":26,"cssClass":"pl-en"},{"start":27,"end":55,"cssClass":"pl-s"},{"start":57,"end":63,"cssClass":"pl-s1"},{"start":65,"end":72,"cssClass":"pl-s1"},{"start":75,"end":85,"cssClass":"pl-c"}],[{"start":20,"end":35,"cssClass":"pl-c"}],[],[],[{"start":12,"end":16,"cssClass":"pl-k"},{"start":17,"end":19,"cssClass":"pl-k"},{"start":21,"end":27,"cssClass":"pl-en"},{"start":28,"end":32,"cssClass":"pl-s1"},{"start":34,"end":37,"cssClass":"pl-c1"},{"start":39,"end":41,"cssClass":"pl-c1"},{"start":42,"end":43,"cssClass":"pl-c1"}],[],[{"start":16,"end":23,"cssClass":"pl-en"},{"start":24,"end":27,"cssClass":"pl-s1"},{"start":29,"end":72,"cssClass":"pl-s"}],[{"start":16,"end":19,"cssClass":"pl-s1"},{"start":20,"end":21,"cssClass":"pl-c1"},{"start":22,"end":28,"cssClass":"pl-en"},{"start":29,"end":34,"cssClass":"pl-s1"},{"start":36,"end":39,"cssClass":"pl-s1"}],[{"start":16,"end":18,"cssClass":"pl-k"},{"start":20,"end":34,"cssClass":"pl-en"},{"start":35,"end":38,"cssClass":"pl-s1"},{"start":40,"end":42,"cssClass":"pl-c1"},{"start":43,"end":58,"cssClass":"pl-c1"}],[],[{"start":20,"end":26,"cssClass":"pl-en"},{"start":27,"end":31,"cssClass":"pl-s"},{"start":33,"end":53,"cssClass":"pl-en"},{"start":54,"end":57,"cssClass":"pl-s1"}],[{"start":20,"end":26,"cssClass":"pl-k"},{"start":27,"end":29,"cssClass":"pl-c1"}],[],[{"start":16,"end":20,"cssClass":"pl-k"}],[],[{"start":20,"end":30,"cssClass":"pl-s1"},{"start":31,"end":32,"cssClass":"pl-c1"},{"start":33,"end":42,"cssClass":"pl-en"},{"start":43,"end":46,"cssClass":"pl-s1"}],[{"start":20,"end":22,"cssClass":"pl-k"},{"start":24,"end":34,"cssClass":"pl-s1"},{"start":38,"end":39,"cssClass":"pl-c1"}],[],[{"start":24,"end":31,"cssClass":"pl-en"},{"start":32,"end":39,"cssClass":"pl-s1"},{"start":41,"end":57,"cssClass":"pl-s"},{"start":59,"end":64,"cssClass":"pl-c1"}],[{"start":24,"end":31,"cssClass":"pl-s1"},{"start":32,"end":33,"cssClass":"pl-c1"},{"start":34,"end":40,"cssClass":"pl-en"},{"start":41,"end":48,"cssClass":"pl-s1"},{"start":79,"end":88,"cssClass":"pl-c"}],[{"start":24,"end":28,"cssClass":"pl-en"},{"start":29,"end":34,"cssClass":"pl-s1"},{"start":36,"end":43,"cssClass":"pl-s1"},{"start":45,"end":52,"cssClass":"pl-s1"},{"start":54,"end":55,"cssClass":"pl-c1"},{"start":79,"end":84,"cssClass":"pl-c"}],[{"start":24,"end":30,"cssClass":"pl-en"},{"start":31,"end":59,"cssClass":"pl-s"},{"start":61,"end":67,"cssClass":"pl-s1"},{"start":69,"end":76,"cssClass":"pl-s1"},{"start":79,"end":89,"cssClass":"pl-c"}],[],[{"start":20,"end":24,"cssClass":"pl-k"}],[],[{"start":24,"end":31,"cssClass":"pl-en"},{"start":32,"end":39,"cssClass":"pl-s1"},{"start":41,"end":55,"cssClass":"pl-s"},{"start":57,"end":62,"cssClass":"pl-c1"}],[{"start":24,"end":31,"cssClass":"pl-s1"},{"start":32,"end":33,"cssClass":"pl-c1"},{"start":34,"end":40,"cssClass":"pl-en"},{"start":41,"end":48,"cssClass":"pl-s1"},{"start":79,"end":88,"cssClass":"pl-c"}],[{"start":24,"end":28,"cssClass":"pl-en"},{"start":29,"end":34,"cssClass":"pl-s1"},{"start":36,"end":43,"cssClass":"pl-s1"},{"start":45,"end":52,"cssClass":"pl-s1"},{"start":54,"end":55,"cssClass":"pl-c1"},{"start":79,"end":84,"cssClass":"pl-c"}],[{"start":24,"end":30,"cssClass":"pl-en"},{"start":31,"end":59,"cssClass":"pl-s"},{"start":61,"end":67,"cssClass":"pl-s1"},{"start":69,"end":76,"cssClass":"pl-s1"},{"start":79,"end":89,"cssClass":"pl-c"}],[],[],[],[],[],[]],"csv":null,"csvError":null,"dependabotInfo":{"showConfigurationBanner":null,"configFilePath":null,"networkDependabotPath":"/remon-nomer66/OMOS/network/updates","dismissConfigurationNoticePath":"/settings/dismiss-notice/dependabot_configuration_notice","configurationNoticeDismissed":false,"repoAlertsPath":"/remon-nomer66/OMOS/security/dependabot","repoSecurityAndAnalysisPath":"/remon-nomer66/OMOS/settings/security_analysis","repoOwnerIsOrg":false,"currentUserCanAdminRepo":true},"displayName":"OMOS_kitchen.c","displayUrl":"https://github.com/remon-nomer66/OMOS/blob/develop/OMOS_kitchen.c?raw=true","headerInfo":{"blobSize":"4.47 KB","deleteInfo":{"deletePath":"https://github.com/remon-nomer66/OMOS/delete/develop/OMOS_kitchen.c","deleteTooltip":"Delete this file"},"editInfo":{"editTooltip":"Edit this file"},"ghDesktopPath":"x-github-client://openRepo/https://github.com/remon-nomer66/OMOS?branch=develop&filepath=OMOS_kitchen.c","gitLfsPath":null,"onBranch":true,"shortPath":"85b7c67","siteNavLoginPath":"/login?return_to=https%3A%2F%2Fgithub.com%2Fremon-nomer66%2FOMOS%2Fblob%2Fdevelop%2FOMOS_kitchen.c","isCSV":false,"isRichtext":false,"toc":null,"lineInfo":{"truncatedLoc":"88","truncatedSloc":"86"},"mode":"file"},"image":false,"isCodeownersFile":null,"isValidLegacyIssueTemplate":false,"issueTemplateHelpUrl":"https://docs.github.com/articles/about-issue-and-pull-request-templates","issueTemplate":null,"discussionTemplate":null,"language":"C","large":false,"loggedIn":true,"newDiscussionPath":"/remon-nomer66/OMOS/discussions/new","newIssuePath":"/remon-nomer66/OMOS/issues/new","planSupportInfo":{"repoIsFork":null,"repoOwnedByCurrentUser":null,"requestFullPath":"/remon-nomer66/OMOS/blob/develop/OMOS_kitchen.c","showFreeOrgGatedFeatureMessage":null,"showPlanSupportBanner":null,"upgradeDataAttributes":null,"upgradePath":null},"publishBannersInfo":{"dismissActionNoticePath":"/settings/dismiss-notice/publish_action_from_dockerfile","dismissStackNoticePath":"/settings/dismiss-notice/publish_stack_from_file","releasePath":"/remon-nomer66/OMOS/releases/new?marketplace=true","showPublishActionBanner":false,"showPublishStackBanner":false},"renderImageOrRaw":false,"richText":null,"renderedFileInfo":null,"tabSize":8,"topBannersInfo":{"overridingGlobalFundingFile":false,"globalPreferredFundingPath":null,"repoOwner":"remon-nomer66","repoName":"OMOS","showInvalidCitationWarning":false,"citationHelpUrl":"https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/creating-a-repository-on-github/about-citation-files","showDependabotConfigurationBanner":null,"actionsOnboardingTip":null},"truncated":false,"viewable":true,"workflowRedirectUrl":null,"symbols":{"timedOut":false,"notAnalyzed":true,"symbols":[]}},"copilotUserAccess":{"canModifyCopilotSettings":true,"canViewCopilotSettings":true,"accessAllowed":true,"hasCFIAccess":true,"hasSubscriptionEnded":false,"business":null},"csrf_tokens":{"/remon-nomer66/OMOS/branches":{"post":"RaRD6u8qwgZZ6bTdeda-E_V-G-pgVM2-g0t_18poPvJHlJc5RmHCdix3MHTOxOpzU96j3nia5bBm2aDFddGyBg"}}},"title":"OMOS/OMOS_kitchen.c at develop · remon-nomer66/OMOS","locale":"en"}