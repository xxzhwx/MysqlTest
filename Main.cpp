#include <stdio.h>
#include <string>
#include "mysql.h"
using std::string;

static MYSQL *__g_conn = NULL;

static char* __g_host = "127.0.0.1";
static char* __g_user = "root";
static char* __g_passwd = "12306";
static char* __g_db = "test";
static unsigned int __g_port = 3306;

void print_error(MYSQL* conn, const char* title)
{
  fprintf(stderr, "%s: \nError %u (%s)\n", title, mysql_errno(conn), mysql_error(conn));
}

void process_result_set(MYSQL* conn, MYSQL_RES* res_set)
{
  MYSQL_ROW row;
  while ((row = mysql_fetch_row(res_set)) != NULL)
  {
    for (unsigned int i = 0; i < mysql_num_fields(res_set); i++)
    {
      if (i > 0)
        fputc('\t', stdout);
      printf("%s", row[i] != NULL ? row[i] : "NULL");
    }
    fputc('\n', stdout);
  }

  if (mysql_errno(conn) != 0)
    print_error(conn, "mysql_fetch_row failed");
  else
    printf("%lu rows returned\n", (unsigned long)mysql_num_rows(res_set));
}

int main(int argc, char *argv[])
{
  printf("Hello, MysqlTest!\n");

  mysql_library_init(0, NULL, NULL);

  if ((__g_conn = mysql_init(NULL)) == NULL)
  {
    fprintf(stderr, "mysql_init failed.\n");
    return -1;
  }

  if (mysql_real_connect(__g_conn, __g_host, __g_user, __g_passwd, __g_db, __g_port, NULL, 0) == NULL)
  {
    fprintf(stderr, "mysql_real_connect failed:\nError %u (%s).\n", 
      mysql_errno(__g_conn), mysql_error(__g_conn));
    mysql_close(__g_conn);
    return -1;
  }

  if (mysql_query(__g_conn, "insert into tb_service (id, name) values ('SERVICE_ID_1', 'service_1')"))
    print_error(__g_conn, "Insert failed");
  else
    printf("Insert success, affected rows:%lu\n", (unsigned long)mysql_affected_rows(__g_conn));

  if (mysql_query(__g_conn, "select * from tb_service"))
  {
    print_error(__g_conn, "Query failed");
  }
  else
  {
    MYSQL_RES* res_set;
    res_set = mysql_store_result(__g_conn);
    if (res_set == NULL)
    {
      print_error(__g_conn, "mysql_store_result failed");
    }
    else
    {
      process_result_set(__g_conn, res_set);
      mysql_free_result(res_set);
    }
  }

  mysql_close(__g_conn);
  mysql_library_end();

  getchar();
  return 0;
}
