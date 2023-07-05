BEGIN TRANSACTION;

INSERT INTO user_t (
    user_id, user_phone, user_name, user_pass
)
VALUES (
    1001, 03012345678, '田中 太郎', 'qwerty1234'
);

INSERT INTO user_point_t (
    user_id, user_point, user_mag
)
VALUES (
    1001, 100, 1.5
);

INSERT INTO user_authority_t (
    user_id, user_authority, user_aff
)
VALUES (
    1001, 5, 10
);

INSERT INTO store_t (
    store_id, store_name, region
)
VALUES (
    10, 'IZAKAYA', 2
);

INSERT INTO store_table_t (
    store_id, desk_num, desk_max
)
VALUES (
    10, 2, 10
);

INSERT INTO order_t (
    store_id, desk_num, menu_id, order_cnt, kitchen_flag, order_date, order_time, account_id
)
VALUES (
    1, 2, 1234, 5, 1, '2023-06-30', '12:34:00', 1001
);

INSERT INTO summary_t (
    store_id, menu_id, order_cnt, order_date, order_time, account_id
)
VALUES (
    1, 1234, 5, '2023-06-30', '12:34:00', 1001
);

INSERT INTO recipe_t (
    menu_id, menu_name, recipe
)
VALUES (
    1234, 'たこわさ', 'たこを切って，わさびを和えます'
);

INSERT INTO menu_price_t (
    menu_id, price
)
VALUES (
    1234, 300
);

INSERT INTO menu_charge_t (
    menu_id, account_id
)
VALUES (
    1234, 1001
);

INSERT INTO push_t (
    menu_id, push, layer
)
VALUES (
    1234, 1, 3
);

INSERT INTO menu_storage_t (
    menu_id, store_id, storage, min_storage
)
VALUES (
    1234, 1, 100, 30
);

INSERT INTO reserve_t (
    account_id, people_num, reserve_date, reserve_time, store_id, desk_num
)
VALUES (
    1001, 8, '2023-08-23', '12:30:00', 1, 2
);

INSERT INTO store_order_t (
    store_id, menu_id, store_order_cnt, store_order_date, store_order_time
)
VALUES (
    1, 1234, 200,  '2023-06-30', '11:34:21'
);

COMMIT;