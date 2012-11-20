package api;

import java.io.UnsupportedEncodingException;
import java.sql.*;
import java.util.*;

public class Db {
	Connection conn;
	Statement stmt;
	ResultSet rs;
	final String ID = "kiosk";
	final String PW = "kiosk";
	final String SERVER = "localhost";
	final String DBNAME = "kiosk";
	
	private HashMap<String, String> insertList = null;
	private ArrayList<String> fieldList = null;
	private HashMap<String, String> filterList = null;
	private ArrayList<String> joinList = null;
	private ArrayList<String> tableList = null;

	private String tableName = "";
	
	public void addField(String value){
		fieldList.add(value);
	}
	
	public void addTable(String name){
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

	public void clear(){
		insertList = new HashMap<String, String>();
		fieldList = new ArrayList<String>();
		filterList = new HashMap<String, String>();
		joinList = new ArrayList<String>();
		tableList = new ArrayList<String>();
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

	public ArrayList<HashMap<String, String>> getData(){						// New Version
		
		ArrayList<HashMap<String, String>> tmpResult = new ArrayList<HashMap<String, String>>();
		
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

			tmpQuery += ";";
									
			
			rs = stmt.executeQuery(tmpQuery);
			rs.last();
			rs.beforeFirst();

			while(rs.next()){
				HashMap<String, String>tmpItem = new HashMap<String, String>();
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
