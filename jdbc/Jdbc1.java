import java.sql.*;

public class Jdbc1 {
    public static void main(String args[]) {

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            System.out.println("Success loading Mysql Driver!");
        }
        catch (Exception e) {
            System.out.print("Error loading Mysql Driver!");
            e.printStackTrace();
        }

        try {
            Connection conn = DriverManager.getConnection(
                //"jdbc:mysql://127.0.0.1:3306/db1?useSSL=false&useServerPrepStmts=false&useCursorFetch=false","root","admin");
                  "jdbc:mysql://127.0.0.1:3306/db1?useSSL=false","root","admin");
            System.out.println("Success connect Mysql server!");
            
            String sql = "select * from t2";
            PreparedStatement stmt = conn.prepareStatement(sql);
            ResultSet rs = stmt.executeQuery();
            ResultSetMetaData data = rs.getMetaData();
            while (rs.next()) {
                for (int i = 1; i <= data.getColumnCount(); i++) {
                //列个数
                int columnCount = data.getColumnCount();
                //列名
                String columnName = data.getColumnName(i);
                //列值
                String columnValue = rs.getString(i);
                Object col_value = rs.getObject(columnName);
                byte[] content = rs.getBytes(i);
                try{
                      String vx1 = new String(content, "UTF-8");
                      System.out.println("getBytes to string: " + vx1);
                      System.out.println("getObject to string: " + col_value.toString());
                } catch (Exception e1) {
                      System.out.println("error1");
                      //throw e1;
                 }
                System.out.println("getObject: " + col_value);
                System.out.println("getString: " + columnValue);

                //列的数据类型
                int columnType = data.getColumnType(i);
                //列的数据类型名
                String columnTypeName = data.getColumnTypeName(i);
                //所在的Catalog名字
                String catalogName = data.getCatalogName(i);
                //对应数据类型的类
                String columnClassName = data.getColumnClassName(i);
                //在数据库中类型的最大字符个数
                int columnDisplaySize = data.getColumnDisplaySize(i);
                //默认的列的标题
                String columnLabel = data.getColumnLabel(i);
                //获得列的模式
                String schemaName = data.getSchemaName(i);
                //某列类型的精确度(类型的长度)
                int precision = data.getPrecision(i);
                //小数点后的位数
                int scale = data.getScale(i);
                //获取某列对应的表名
                String tableName = data.getTableName(i);
                //是否自动递增
                boolean isAutoInctement = data.isAutoIncrement(i);
                //在数据库中是否为货币型
                boolean isCurrency = data.isCurrency(i);
                //是否为空
                int isNullable = data.isNullable(i);
                //是否为只读
                boolean isReadOnly = data.isReadOnly(i);
                //能否出现在where中
                boolean isSearchable = data.isSearchable(i);
                System.out.println("获得列" + i + "的字段名称:" + columnName);
                System.out.println("获得列" + i + "的字段值:" + columnValue);
                System.out.println("获得列" + i + "的类型,返回SqlType中的编号:" + columnType);
                System.out.println("获得列" + i + "的数据类型名:" + columnTypeName);
                System.out.println("获得列" + i + "所在的Catalog名字:" + catalogName);
                System.out.println("获得列" + i + "对应数据类型的类:" + columnClassName);
                System.out.println("获得列" + i + "在数据库中类型的最大字符个数:" + columnDisplaySize);
                System.out.println("获得列" + i + "的默认的列的标题:" + columnLabel);
                System.out.println("获得列" + i + "的模式:" + schemaName);
                System.out.println("获得列" + i + "类型的精确度(类型的长度):" + precision);
                System.out.println("获得列" + i + "小数点后的位数:" + scale);
                System.out.println("获得列" + i + "对应的表名:" + tableName);
                System.out.println("获得列" + i + "是否自动递增:" + isAutoInctement);
                System.out.println("获得列" + i + "在数据库中是否为货币型:" + isCurrency);
                System.out.println("获得列" + i + "是否为空:" + isNullable);
                System.out.println("获得列" + i + "是否为只读:" + isReadOnly);
                System.out.println("获得列" + i + "能否出现在where中:" + isSearchable);
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}
