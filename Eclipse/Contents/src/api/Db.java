package api;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class Db {
	Connection conn;
	Statement stmt;
	ResultSet rs;
	final String ID = "kiosk";
	final String PW = "kiosk";
	final String SERVER = "localhost";
	final String DBNAME = "kiosk";
	
	private Map<String, String> insertList = null;
	private List<String> fieldList = null;
	private Map<String, String> filterList = null;
	private List<String> joinList = null;
	private List<String> tableList = null;

	private String postfixQuery = "";
	
	public void addField(String value){
		fieldList.add(value);
	}
	
	public void setTable(String name){
		tableList.add(name);
	}
	
	public void setInsert(String name, String value){
		insertList.put(name, value);
	}
	
	public void addFilter(String name, String value){
		filterList.put(name, value);
	}
	
	public void addJoin(String leftValue, String rightValue){
		joinList.add(leftValue);
		joinList.add(rightValue);
	}
	
	public void setPostfixQuery(String postfix){
		postfixQuery = postfix;
	}

	public void clear(){
		insertList = new HashMap<String, String>();
		fieldList = new ArrayList<String>();
		filterList = new HashMap<String, String>();
		joinList = new ArrayList<String>();
		tableList = new ArrayList<String>();
		postfixQuery = "";
	}
	
	void Connection(){
		try {
			Class.forName("com.mysql.jdbc.Driver");
			conn = DriverManager.getConnection(
					"jdbc:mysql://" + SERVER + "/" + DBNAME + "?useUnicode=true&characterEncoding=utf8", 
					ID, 
					PW);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public Db(){
		clear();
		
		Connection();
	}

	public List<Map<String, String>> getData(){						// New Version
		
		List<Map<String, String>> tmpResult = new ArrayList<Map<String, String>>();
		
		try {
			stmt = conn.createStatement();
			String tmpQuery = "SELECT ";
			for(String fieldName : fieldList){
				tmpQuery += "" + fieldName + ", ";
			}
			tmpQuery = tmpQuery.substring(0, tmpQuery.length() - 2);
			tmpQuery += " FROM ";
			
			tmpQuery += "`" + tableList.get(0) + "`";
			
			if(!joinList.isEmpty()){
				tmpQuery += " LEFT OUTER JOIN `" + tableList.get(1) + "` ON ";
				tmpQuery += "" + joinList.get(0) + " = " + joinList.get(1);
			}
			
			if(!filterList.isEmpty())
				tmpQuery += " WHERE ";

			
			if(!filterList.isEmpty()){
				for(String filterItem : filterList.keySet()){
					tmpQuery += "" + filterItem + " = \"" + filterList.get(filterItem) + "\" AND ";
				}
				tmpQuery = tmpQuery.substring(0, tmpQuery.length() - 4);
			}

			tmpQuery += " " + postfixQuery + ";";
									
			
			rs = stmt.executeQuery(tmpQuery);
			rs.last();
			rs.beforeFirst();

			while(rs.next()){
				Map<String, String>tmpItem = new HashMap<String, String>();
				for(String fieldName : fieldList){
					tmpItem.put(fieldName, rs.getString(fieldName));
				}
				tmpResult.add(tmpItem);
			}
		} catch (SQLException e) {
			e.printStackTrace();
			try {
				conn.close();
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Connection();
		}
		return tmpResult;
	}
	
	
	
	public int deleteData(){					// New Version
		int queryResult = 0;
				
		try {
			stmt = conn.createStatement();
			String tmpQuery = "DELETE";
			tmpQuery += " FROM `" + tableList.get(0) + "`";
			
			tmpQuery += " WHERE ";
			for(String filterItem : filterList.keySet()){
				tmpQuery += "`" + filterItem + "` = \"" + filterList.get(filterItem) + "\" AND ";
			}
			tmpQuery = tmpQuery.substring(0, tmpQuery.length() - 4);
			tmpQuery += ";";
			
			queryResult = stmt.executeUpdate(tmpQuery);
		} catch (SQLException e) {
			e.printStackTrace();
			try {
				conn.close();
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Connection();
		}
		return queryResult;
	}
	
	public int putData(){
		int queryResult = 0;
		
		try{
			stmt = conn.createStatement();
			
			String tmpQuery = "INSERT INTO " + tableList.get(0) + "(";
			String fieldQuery = "";
			String dataQuery = "";
			
			for(String fieldName: insertList.keySet()){
				fieldQuery += "`" + fieldName + "`, ";
				if(insertList.get(fieldName).getClass() == String.class){
					dataQuery += "\"" + insertList.get(fieldName) + "\", ";
				} else {
					dataQuery += "" + insertList.get(fieldName) + ", ";
				}
			}
			
			fieldQuery = fieldQuery.substring(0, fieldQuery.length() - 2);
			dataQuery = dataQuery.substring(0, dataQuery.length() - 2);
			
			tmpQuery += fieldQuery + ") values (" + dataQuery + ");";
			
			tmpQuery = new String(tmpQuery.getBytes("8859_1"), "UTF-8");
			queryResult = stmt.executeUpdate(tmpQuery);
		} catch (Exception e){
			e.printStackTrace();
			try {
				conn.close();
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Connection();
		}
		
		
		return queryResult;
	}
	
	public int modifyData(){
		int queryResult = 0;
		
		try{
			stmt = conn.createStatement();
			
			String tmpQuery = "UPDATE " + tableList.get(0);
			
			tmpQuery += " SET ";
			
			for(String fieldName: insertList.keySet()){
				tmpQuery  += "`" + fieldName + "` = ";
				if(insertList.get(fieldName).getClass() == String.class){
					tmpQuery += "\"" + insertList.get(fieldName) + "\"";
				} else {
					tmpQuery += "" + insertList.get(fieldName) + "";
				}
				tmpQuery += ", ";
			}
			
			tmpQuery = tmpQuery.substring(0, tmpQuery.length() - 2);
			
			
			tmpQuery += " WHERE ";
			for(String filterItem : filterList.keySet()){
				tmpQuery += "`" + filterItem + "` = \"" + filterList.get(filterItem) + "\" AND ";
			}
			tmpQuery = tmpQuery.substring(0, tmpQuery.length() - 4);
			tmpQuery += ";";
			
			tmpQuery = new String(tmpQuery.getBytes("8859_1"), "UTF-8");
			queryResult = stmt.executeUpdate(tmpQuery);
		} catch (Exception e){
			e.printStackTrace();
			try {
				conn.close();
			} catch (SQLException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Connection();
		}
		
		return queryResult;
	}
	
	public void destroy(){
		try{
			conn.close();
			System.out.println("DB Closed");
		}catch(Exception e){
		}
	}
}
