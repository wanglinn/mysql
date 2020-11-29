# 1. 创建表语句
```
mysql version: 5.7.26

create table t21(id21 int, id22 int, id23 int);
insert into t21 values(1,11,21);
insert into t21 values(2,12,22);

```
# 2. 执行语句 select sql_no_cache * from t21 where id21=1 and id22 = 11 and id23=21;

# 2.1在 do_select 处打断点，堆栈信息
```
#0  do_select (join=0x7f560c134c70) at /home/wl/software/mysql5726/sql/sql_executor.cc:878
#1  0x00000000014f2773 in JOIN::exec (this=0x7f560c134c70) at /home/wl/software/mysql5726/sql/sql_executor.cc:199
#2  0x000000000158baca in handle_query (thd=0x7f560c000b90, lex=0x7f560c002eb0, result=0x7f560c134738, added_options=0, removed_options=0)
    at /home/wl/software/mysql5726/sql/sql_select.cc:184
#3  0x0000000001541784 in execute_sqlcom_select (thd=0x7f560c000b90, all_tables=0x7f560c006e00) at /home/wl/software/mysql5726/sql/sql_parse.cc:5151
#4  0x000000000153b165 in mysql_execute_command (thd=0x7f560c000b90, first_level=true) at /home/wl/software/mysql5726/sql/sql_parse.cc:2816
#5  0x00000000015426e8 in mysql_parse (thd=0x7f560c000b90, parser_state=0x7f5632c06690) at /home/wl/software/mysql5726/sql/sql_parse.cc:5577
#6  0x0000000001537fb5 in dispatch_command (thd=0x7f560c000b90, com_data=0x7f5632c06df0, command=COM_QUERY) at /home/wl/software/mysql5726/sql/sql_parse.cc:1484
#7  0x0000000001536e1e in do_command (thd=0x7f560c000b90) at /home/wl/software/mysql5726/sql/sql_parse.cc:1025
#8  0x0000000001667862 in handle_connection (arg=0x50f0690) at /home/wl/software/mysql5726/sql/conn_handler/connection_handler_per_thread.cc:306
#9  0x0000000001cf333e in pfs_spawn_thread (arg=0x51cf8e0) at /home/wl/software/mysql5726/storage/perfschema/pfs.cc:2190
#10 0x00007f563e85bdd5 in start_thread () from /lib64/libpthread.so.0
#11 0x00007f563d52102d in clone () from /lib64/libc.so.6

```

# 2.2 查看语句对应的lex信息

```
(gdb) p *lex
$6 = (LEX) {
  <Query_tables_list> = {
    sql_command = SQLCOM_SELECT,   -- 语句命令类型
    query_tables = 0x7f560c006e00,   -- 涉及的表
    query_tables_last = 0x7f560c006e08, 
    query_tables_own_last = 0x0, 
    sroutines = {
      key_offset = 0, 
      key_length = 0, 
      blength = 0, 
      records = 0, 
      flags = 0, 
      array = {
        buffer = 0x0, 
        elements = 0, 
        max_element = 0, 
        alloc_increment = 0, 
        size_of_element = 0, 
        m_psi_key = 0
      }, 
      get_key = 0x0, 
      free = 0x0, 
      charset = 0x0, 
      hash_function = 0x0, 
      m_psi_key = 0
    }, 
    sroutines_list = {
      <Sql_alloc> = {<No data fields>}, 
      members of SQL_I_List<Sroutine_hash_entry>: 
      elements = 0, 
      first = 0x0, 
      next = 0x7f560c002f50
    }, 
    sroutines_list_own_last = 0x7f560c002f50, 
    sroutines_list_own_elements = 0, 
    lock_tables_state = Query_tables_list::LTS_LOCKED,   -- 表的锁状态，分为 LTS_NOT_LOCKED，LTS_LOCKED
    table_count = 1, 
    static BINLOG_STMT_UNSAFE_ALL_FLAGS = 2097151, 
    static binlog_stmt_unsafe_errcode = {1668, 1670, 1671, 1672, 1673, 1674, 1675, 1692, 1693, 1714, 1715, 1722, 1716, 1717, 1718, 1723, 1719, 1724, 1727, 3006, 
      3199}, 
    binlog_stmt_flags = 0, 
    stmt_accessed_table_flag = 1, 
    using_match = false
  }, 
  members of LEX: 
  _vptr.LEX = 0x2c280d0 <vtable for LEX+16>, 
  unit = 0x7f560c005c28, 
  select_lex = 0x7f560c005940,   --- 对应select语句的 st_select_lex 类信息
  all_selects_list = 0x7f560c005940, --
  m_current_select = 0x7f560c005940, -- 
  length = 0x0, 
  dec = 0x0, 
  change = 0x0, 
  name = {
    str = 0x0, 
    length = 0
  }, 
  help_arg = 0x0, 
  to_log = 0x0, 
  x509_subject = 0x0, 
  x509_issuer = 0x0, 
  ssl_cipher = 0x0, 
  wild = 0x0, 
  exchange = 0x0, 
  result = 0x0, 
  default_value = 0x0, 
  on_update_value = 0x0, 
  comment = {
    str = 0x0, 
    length = 0
  }, 
  ident = {
    str = 0x0, 
    length = 0
  }, 
  grant_user = 0x0, 
  alter_password = {
    update_password_expired_fields = false, 
    update_password_expired_column = false, 
    use_default_password_lifetime = false, 
    expire_after_days = 0, 
    update_account_locked_column = false, 
    account_locked = false
  }, 
  thd = 0x7f560c000b90, 
  gcol_info = 0x0, 
  opt_hints_global = 0x0, 
  ... 
  charset = 0x0, 
  insert_table = 0x0, 
  insert_table_leaf = 0x0, 
  create_view_select = {
    str = 0x0, 
    length = 0
  }, 
  raw_trg_on_table_name_begin = 0x0, 
  raw_trg_on_table_name_end = 0x0, 
  trg_ordering_clause_begin = 0x0, 
  trg_ordering_clause_end = 0x0, 
  part_info = 0x0, 
  definer = 0x0, 
  col_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003158, 
      elements = 0
    }, <No data fields>}, 
  ref_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003170, 
      elements = 0
    }, <No data fields>}, 
  interval_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003188, 
      elements = 0
    }, <No data fields>}, 
  users_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c0031a0, 
      elements = 0
    }, <No data fields>}, 
  columns = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c0031b8, 
      elements = 0
    }, <No data fields>}, 
  bulk_insert_row_cnt = 0, 
  load_field_list 
    ...
  purge_value_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003238, 
      elements = 0
    }, <No data fields>}, 
  kill_value_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003250, 
      elements = 0
    }, <No data fields>}, 
  call_value_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003268, 
      elements = 0
    }, <No data fields>}, 
  handler_insert_list = 0x0, 
  var_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003288, 
      elements = 0
    }, <No data fields>}, 
  set_var_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c0032a0, 
      elements = 0
    }, <No data fields>}, 
  param_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c0032b8, 
      elements = 0
    }, <No data fields>}, 
  view_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c0032d0, 
      elements = 0
    }, <No data fields>}, 
  insert_update_values_map = 0x0, 
  context_stack = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x7f560c006260, 
      last = 0x7f560c006260, 
      elements = 1
    }, <No data fields>}, 
  proc_analyse = 0x0, 
  auxiliary_table_list = {
    <Sql_alloc> = {<No data fields>}, 
    members of SQL_I_List<TABLE_LIST>: 
    elements = 0, 
    first = 0x0, 
    next = 0x7f560c003318
  }, 
  save_list = {
    <Sql_alloc> = {<No data fields>}, 
    members of SQL_I_List<TABLE_LIST>: 
    elements = 0, 
    first = 0x0, 
    next = 0x7f560c003330
  }, 
  last_field = 0x7f560c006a10, 
  in_sum_func = 0x0, 
  udf = {
    ...
  }, 
  check_opt = {
    flags = 0, 
    sql_flags = 0, 
    key_cache = 0x0
  }, 
  create_info = {
    table_charset = 0x0, 
    ...
  }, 
  key_create_info = {
    algorithm = HA_KEY_ALG_UNDEF, 
     ...
  }, 
  mi = {
    host = 0x0, 
    user = 0x0, 
    password = 0x0, 
    ...
  }, 
    ...
  }, 
  mqh = {
    questions = 0, 
    updates = 0, 
    conn_per_hour = 0, 
    user_conn = 0, 
    specified_limits = 0
  }, 
  reset_slave_info = {
    all = false
  }, 
  type = 0, 
  allow_sum_func = 0, 
  ...
  slave_thd_opt = 0, 
  ...
  alter_info = {
    static ALTER_ADD_COLUMN = 1, 
    ...
    static ALTER_RENAME_INDEX = 67108864, 
    static ALTER_UPGRADE_PARTITIONING = 134217728, 
    ...
    flags = 0, 
    keys_onoff = Alter_info::LEAVE_AS_IS, 
    partition_names = {
      <base_list> = {
        <Sql_alloc> = {<No data fields>}, 
        members of base_list: 
        first = 0x2d1a5b0 <end_of_list>, 
        last = 0x7f560c0037b8, 
        elements = 0
      }, <No data fields>}, 
    num_parts = 0, 
    requested_algorithm = Alter_info::ALTER_TABLE_ALGORITHM_DEFAULT, 
    requested_lock = Alter_info::ALTER_TABLE_LOCK_DEFAULT, 
    with_validation = Alter_info::ALTER_VALIDATION_DEFAULT
  }, 
  create_last_non_select_table = 0x7f560c006288, 
  prepared_stmt_name = {
    str = 0x0, 
    length = 0
  }, 
  prepared_stmt_code = {
    str = 0x0, 
    length = 0
  }, 
  prepared_stmt_code_is_varref = false, 
  prepared_stmt_params = {            --- prepare 参数
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c003810, 
      elements = 0
    }, <No data fields>}, 
  sphead = 0x0,                  -- spe 存储过程相关变量信息
  spname = 0x0, 
  sp_lex_in_use = false, 
  all_privileges = false, 
  proxy_priv = false, 
  is_set_password_sql = false, 
  contains_plaintext_password = false, 
  keep_diagnostics = DA_KEEP_PARSE_ERROR, 
  m_broken = false, 
  sp_current_parsing_ctx = 0x0, 
  sp_chistics = {
    comment = {
      str = 0x0, 
      length = 0
    }, 
    suid = SP_IS_DEFAULT_SUID, 
    detistic = false, 
    daccess = SP_DEFAULT_ACCESS
  }, 
  event_parse_data = 0x0, 
  only_view = false, 
  create_view_suid = 0 '\000', 
  stmt_definition_begin = 0x0, 
  stmt_definition_end = 0x0, 
  use_only_table_context = false, 
  alter_tablespace_info = 0x0, 
  is_lex_started = true, 
  in_update_value_clause = false, 
  used_tables = 1, 
  explain_format = 0x0, 
  max_execution_time = 0, 
  binlog_need_explicit_defaults_ts = false
}
```
## 2.3 查看 lex->select_lex 内容
```
(gdb) p lex->select_lex
$26 = (SELECT_LEX *) 0x7f560c005940
(gdb) p *lex->select_lex
$27 = (st_select_lex) {
  <Sql_alloc> = {<No data fields>}, 
  members of st_select_lex: 
  _vptr.st_select_lex = 0x2c280b0 <vtable for st_select_lex+16>, 
  next = 0x0, 
  prev = 0x7f560c005c40, 
  master = 0x7f560c005c28, 
  slave = 0x0, 
  link_next = 0x0, 
  link_prev = 0x7f560c002f90, 
  m_query_result = 0x7f560c134738, 
  m_base_options = 0, 
  m_active_options = 2147748608, 
  sql_cache = st_select_lex::SQL_NO_CACHE,   -- 是否从缓存中获取结果数据
  uncacheable = 0 '\000', 
  linkage = UNSPECIFIED_TYPE, 
  no_table_names_allowed = false, 
  context = {
    <Sql_alloc> = {<No data fields>}, 
    members of Name_resolution_context: 
    outer_context = 0x0, 
    next_context = 0x0, 
    table_list = 0x7f560c006e00, 
    first_name_resolution_table = 0x7f560c006e00, 
    last_name_resolution_table = 0x0, 
    select_lex = 0x7f560c005940, 
    view_error_handler = false, 
    view_error_handler_arg = 0x0, 
    resolve_in_select_list = true, 
    security_ctx = 0x0
  }, 
  first_context = 0x7f560c0059a0, 
  resolve_place = st_select_lex::RESOLVE_NONE, 
  resolve_nest = 0x0, 
  semijoin_disallowed = false, 
  db = 0x0, 
  m_where_cond = 0x7f560c006920, 
  m_having_cond = 0x0, 
  cond_value = Item::COND_OK, 
  having_value = Item::COND_UNDEF, 
  parent_lex = 0x7f560c002eb0, 
  olap = UNSPECIFIED_OLAP_TYPE, 
  table_list = {                     -- select 语句对应的表信息
    <Sql_alloc> = {<No data fields>}, 
    members of SQL_I_List<TABLE_LIST>: 
    elements = 1,                    -- 表的个数
    first = 0x7f560c006e00, 
    next = 0x7f560c006e00
  }, 
  group_list = {
    <Sql_alloc> = {<No data fields>}, 
    members of SQL_I_List<st_order>: 
    elements = 0, 
    first = 0x0, 
    next = 0x7f560c005a60
  }, 
  group_list_ptrs = 0x0, 
  item_list = {              --- 对应的查询语句的结果列信息
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x7f560c0063b8, 
      last = 0x7f560c134b18, 
      elements = 3            --- 结果列的个数
    }, <No data fields>}, 
  is_item_list_lookup = true, 
  hidden_group_field_count = 0, 
  fields_list = @0x7f560c005a78, 
  all_fields = {              --- 结果列的信息
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x7f560c0063b8, 
      last = 0x7f560c0063b8, 
      elements = 3
    }, <No data fields>}, 
  ftfunc_list = 0x7f560c005ac0, 
  ftfunc_list_alloc = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c005ac0, 
      elements = 0
    }, <No data fields>}, 
  join = 0x7f560c134c70,    -- 对应的select 语句的join信息
  top_join_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x7f560c007390, 
      last = 0x7f560c007390, 
      elements = 1
    }, <No data fields>}, 
  join_list = 0x7f560c005ae0,   -- join对应的list
  embedding = 0x0, 
  sj_nests = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c005b08, 
      elements = 0
    }, <No data fields>}, 
  leaf_tables = 0x7f560c006e00, 
  leaf_table_count = 1, 
  derived_table_count = 0, 
  materialized_derived_table_count = 0, 
  has_sj_nests = false, 
  partitioned_table_count = 0, 
  order_list = {                    -- order by 信息
    <Sql_alloc> = {<No data fields>}, 
    members of SQL_I_List<st_order>: 
    elements = 0, 
    first = 0x0, 
    next = 0x7f560c005b48
  }, 
  order_list_ptrs = 0x0, 
  select_limit = 0x7f560c134690,    --  limit 信息
  offset_limit = 0x0, 
  ref_pointer_array = {
    m_array = 0x7f560c134b28,       -- 看内容是表的列信息
    m_size = 40
  }, 
  ref_ptrs = {
    m_array = 0x7f560c134b28, 
    m_size = 8
  }, 
  select_n_having_items = 1, 
  cond_count = 9, 
  between_count = 0, 
  max_equal_elems = 1, 
  select_n_where_fields = 4, 
  parsing_place = CTX_NONE, 
  ...
  inner_refs_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x2d1a5b0 <end_of_list>, 
      last = 0x7f560c005bd0, 
      elements = 0
    }, <No data fields>}, 
  explicit_limit = false, 
  subquery_in_having = false, 
  first_execution = false, 
  sj_pullout_done = true, 
  exclude_from_table_unique_test = false, 
  allow_merge_derived = true, 
  prev_join_using = 0x0, 
  select_list_tables = 1, 
  outer_join = 0, 
  opt_hints_qb = 0x0, 
  m_agg_func_used = false, 
  m_json_agg_func_used = false, 
  static type_str = {0x20c4af7 "NONE", 0x20c4afc "PRIMARY", 0x20c4b04 "SIMPLE", 0x20c4b0b "DERIVED", 0x20c4b13 "SUBQUERY", 0x20c4b1c "UNION", 
    0x20c4b22 "UNION RESULT", 0x20c4b2f "MATERIALIZED"}, 
  sj_candidates = 0x0, 
  hidden_order_field_count = 0
}


```
查看表信息  

```
(gdb) p lex->select_lex
$87 = (SELECT_LEX *) 0x7f560c005940
(gdb) p lex->select_lex->table_list 
$88 = {
  <Sql_alloc> = {<No data fields>}, 
  members of SQL_I_List<TABLE_LIST>: 
  elements = 1, 
  first = 0x7f560c006e00, 
  next = 0x7f560c006e00
}

gdb) p *lex->select_lex->table_list->first 
$91 = {
  next_local = 0x0, 
  next_global = 0x0, 
  prev_global = 0x7f560c002ec0, 
  db = 0x7f560c007388 "db1",   -- 库名
  table_name = 0x7f560c0063f8 "t21",   -- 表名
  alias = 0x7f560c006df8 "t21",   -- 表的别名
  target_tablespace_name = {  -- 空间名
    str = 0x0, 
    length = 0
  }, 
  schema_table_name = 0x0, 
  option = 0x0, 
  opt_hints_table = 0x0, 
  opt_hints_qb = 0x0, 
  m_tableno = 0, 
  m_map = 1, 
  m_join_cond = 0x0, 
  m_sj_cond = 0x0, 
  sj_inner_tables = 0, 
  natural_join = 0x0, 
  is_natural_join = false, 
  join_using_fields = 0x0, 
  join_columns = 0x0, 
  is_join_columns_complete = false, 
  next_name_resolution_table = 0x0, 
  index_hints = 0x0, 
  table = 0x7f560c132940, 
  table_id = {
    static TABLE_ID_MAX = 281474976710655, 
    m_id = 0
  }, 
  derived_result = 0x0, 
  correspondent_table = 0x0, 
  derived = 0x0, 
  schema_table = 0x0, 
  schema_select_lex = 0x0, 
  schema_table_reformed = false, 
  schema_table_param = 0x0, 
  select_lex = 0x7f560c005940, 
  ...
  select_stmt = {
    str = 0x0, 
    length = 0
  }, 
    ...
  }, 
  file_version = 0, 
  updatable_view = 0, 
  algorithm = 0, 
  view_suid = 0, 
  with_check = 0, 
  effective_algorithm = VIEW_ALGORITHM_UNDEFINED, 
  grant = {
    grant_table = 0x0, 
    ...
  }, 
  engine_data = 0, 
  callback_func = 0x0, 
  lock_type = TL_READ_DEFAULT,   -- 表的锁状态
  outer_join = 0, 
  shared = 0, 
  db_length = 3, 
  table_name_length = 3, 
  m_updatable = true, 
  m_insertable = true, 
  ...
  open_type = OT_TEMPORARY_OR_BASE, 
  contain_auto_increment = false, 
  check_option_processed = false, 
  replace_filter_processed = false, 
  required_type = FRMTYPE_ERROR, 
  timestamp_buffer = '\000' <repeats 19 times>, 
  prelocking_placeholder = false, 
  open_strategy = TABLE_LIST::OPEN_NORMAL, 
  internal_tmp_table = false, 
  is_alias = false, 
  is_fqtn = false, 
  view_creation_ctx = 0x0, 
  view_client_cs_name = {
    str = 0x0, 
    length = 0
  }, 
  view_connection_cl_name = {
    str = 0x0, 
    length = 0
  }, 
  view_body_utf8 = {
    str = 0x0, 
    length = 0
  }, 
  derived_key_list = {
    <base_list> = {
      <Sql_alloc> = {<No data fields>}, 
      members of base_list: 
      first = 0x0, 
      last = 0x0, 
      elements = 0
    }, <No data fields>}, 
  trg_event_map = 0 '\000', 
  i_s_requested_object = 0, 
  has_db_lookup_value = false, 
  has_table_lookup_value = false, 
  table_open_method = 0, 
  schema_table_state = NOT_PROCESSED, 
  mdl_request = {            --- 元数据信息
    type = MDL_SHARED_READ, 
    duration = MDL_TRANSACTION, 
    next_in_list = 0x0, 
    prev_in_list = 0x0, 
    ticket = 0x7f560c020850, 
    key = {
      m_length = 9, 
      m_db_name_length = 3, 
      m_ptr = "\003db1\000t21", '\000' <repeats 378 times>, 
      static m_namespace_to_wait_state_name = {{
          m_key = 0, 
          m_name = 0x1f47bce "Waiting for global read lock", 
          m_flags = 0
        }, {
          ...
          m_key = 0, 
          m_name = 0x1f47d30 "Waiting for locking service lock", 
          m_flags = 0
        }}
    }, 
    m_src_file = 0x20df0b0 "/home/wl/software/mysql5726/sql/sql_parse.cc", 
    m_src_line = 6012
  }, 
  view_no_explain = false, 
  partition_names = 0x0, 
  m_join_cond_optim = 0x0, 
  cond_equal = 0x0, 
  optimized_away = false, 
  derived_keys_ready = false, 
  m_table_ref_type = TABLE_REF_BASE_TABLE,  -- 表的类型
  m_table_ref_version = 113
}

```

查看结果列信息  
```
(gdb) p ((Item_field *)((list_node *)lex->select_lex->item_list->first)->info)
$64 = (Item_field *) 0x7f560c134760
(gdb) p ((Item_field *)((list_node *)lex->select_lex->item_list->first)->info)->item_name
$65 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c134890 "id21", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p ((Item_field *)((list_node *)lex->select_lex->item_list->first->next)->info)->item_name
$66 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c1349c8 "id22", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p ((Item_field *)((list_node *)lex->select_lex->item_list->first->next->next)->info)->item_name
$67 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c134b10 "id23", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
```

## 2.4 查看 where 条件  

```
(gdb) p lex->select_lex->m_where_cond 
$94 = (Item_cond_and *) 0x7f560c006920

如何判断调用的函数是 Item_cond_and ？

(gdb) p lex->select_lex->m_where_cond->type()
$73 = Item::COND_ITEM                     -> 然后使用强制类型转换，找到functype()
(gdb) p ((Item_cond  *)lex->select_lex->m_where_cond)->functype()
$74 = Item_func::COND_AND_FUNC


(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->list
$107 = {
  <base_list> = {
    <Sql_alloc> = {<No data fields>}, 
    members of base_list: 
    first = 0x7f560c135ff8, 
    last = 0x7f560c0069f8, 
    elements = 3
  }, <No data fields>}

(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->args
$108 = (Item **) 0x7f560c0069c8

(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()
$8 = (List<Item> *) 0x7f560c0069f8
(gdb) p *((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()
$9 = {
  <base_list> = {
    <Sql_alloc> = {<No data fields>}, 
    members of base_list: 
    first = 0x7f560c135ff8, 
    last = 0x7f560c0069f8, 
    elements = 3
  }, <No data fields>}

(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->elements
$15 = 3
(gdb) 

(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->first
$14 = (list_node *) 0x7f560c135ff8

(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()
$22 = (Item_func_eq *) 0x7f560c135e58
(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->type()
$23 = Item::FUNC_ITEM



(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head())->args[0]->item_name
$39 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c006a68 "id23", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head())->args[1]->item_name
$40 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c006b30 "21", 
      m_length = 2
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}


(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->next)->args[0]->item_name
$42 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c0066f8 "id22", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->next)->args[1]->item_name
$43 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c0067c0 "11", 
      m_length = 2
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}


(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->next->next)->args[0]->item_name
$45 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c0064c8 "id21", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->next->next)->args[1]->item_name
$46 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c006590 "1", 
      m_length = 1
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}

查看where 条件 中 每个表达式中用的函数 
(gdb) p ((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()->type()
$77 = Item::FUNC_ITEM
然后调用 Item_func 强制转换得到functype()
(gdb) p ((Item_func *)(((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()))->functype()
$83 = Item_func::EQ_FUNC
那么 (((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()) 对应的类型 Item_func::EQ_FUNC
即 Item_func_eq

(gdb) p ((Item_func_eq *)(((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()))->args[0]->item_name
$98 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c006a68 "id23", 
      m_length = 4
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}
(gdb) p ((Item_func_eq *)(((Item_cond_and *)(lex->select_lex->m_where_cond ))->argument_list()->head()))->args[1]->item_name
$99 = {
  <Name_string> = {
    <Simple_cstring> = {
      m_str = 0x7f560c006b30 "21", 
      m_length = 2
    }, <No data fields>}, 
  members of Item_name_string: 
  m_is_autogenerated = true
}


```
