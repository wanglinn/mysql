```
import java.sql.*;
 
public class Jdbc3 {
 
    static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";  
    static final String DB_URL = "jdbc:mysql:loadbalance://127.0.0.1:3306,127.0.0.1:13306/db1?"
                                  + "useSSL=false&useServerPrepStmts=true&cachePrepStmts=false";
 
    //8.0 及以上版本
    //static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
    //static final String DB_URL = "jdbc:mysql://127.0.0.1:3306/RUNOOB?useSSL=false&serverTimezone=UTC";

    static final String USER = "root";
    static final String PASS = "admin";

    public static void printfunc(ResultSet rs) throws SQLException ,Exception
    {
        try{
            while(rs.next()){
                int id1  = rs.getInt("id1");
                String id2 = rs.getString("id2");

                System.out.print("id1: " + id1);
                System.out.print(", id2: " + id2);
                System.out.print("\n");
            }
            rs.close();
         }catch(SQLException se){
            throw se;
        }catch(Exception e){
            throw e;
        }
    }

    public static void execPrepare()
    {
        Connection conn = null;
        PreparedStatement pstmt = null;
        String sql;

        try{
            conn = DriverManager.getConnection(DB_URL,USER,PASS);
            System.out.println("connect exec prepare!");

            sql = "select id1, id2 from t1 where id1 = ? for update;";
            pstmt = conn.prepareStatement(sql);

            int x1 = 1;
            pstmt.setInt(1, x1);

            ResultSet rs = pstmt.executeQuery();
            printfunc(rs);

            x1 = 2;
            pstmt.setInt(1, x1);
            rs = pstmt.executeQuery();
            printfunc(rs);

            x1 = 101;
            pstmt.setInt(1, x1 );
            rs = pstmt.executeQuery();
            printfunc(rs);

            x1 = 102;
            pstmt.setInt(1, x1 );
            rs = pstmt.executeQuery();
            printfunc(rs);

            pstmt.close();
            conn.close();
        }catch(SQLException se){
            se.printStackTrace();
        }catch(Exception e){
            e.printStackTrace();
        }finally{
            try{
                if(pstmt!=null)
                    pstmt.close();
            }catch(SQLException se2){
                se2.printStackTrace();
            }
            try{
                if(conn!=null)
                    conn.close();
            }catch(SQLException se){
                se.printStackTrace();
            }
        }

        System.out.println("ok!");
    }
 
    public static void execNormal()
    {
        Connection conn = null;
        Statement stmt = null;
        String sql;

        try{
            conn = DriverManager.getConnection(DB_URL,USER,PASS);
            System.out.println("connect exec normal!");

            sql = "select id1, id2 from t1 where id1 = 1 for update;";
            stmt = conn.createStatement();

            ResultSet rs = stmt.executeQuery(sql);
            printfunc(rs);

            stmt.close();
            conn.close();
        }catch(SQLException se){
            se.printStackTrace();
        }catch(Exception e){
            e.printStackTrace();
        }finally{
            try{
                if(stmt!=null)
                    stmt.close();
            }catch(SQLException se2){
                se2.printStackTrace();
            }
            try{
                if(conn!=null)
                    conn.close();
            }catch(SQLException se){
                se.printStackTrace();
            }
        }

        System.out.println("ok!");
    }

    public static void main(String[] args) {
        Connection conn = null;
        //Statement stmt = null;
        PreparedStatement pstmt = null;
        String sql;

        try{
            Class.forName(JDBC_DRIVER);
            System.out.println("Success loading Mysql Driver!");

            execPrepare();
            execNormal();
            execPrepare();
            execPrepare();
            execPrepare();

        }catch(Exception e){
            e.printStackTrace();
        }finally{}

    }
}
```
