package api;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

/**
 * Servlet implementation class Voice
 */
@WebServlet("/api/voice")
public class Voice extends HttpServlet {
	private static final long serialVersionUID = 1L;
	Db oDb;
       
    /**
     * @see HttpServlet#HttpServlet()
     */
    public Voice() {
        super();
        oDb = new Db();
        // TODO Auto-generated constructor stub
    }

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		final String VOICE_COMMAND = "voice_command";
		final String MENU_RESULT = "menu_result";
		final String VOICE_SUCCESS = "voice_success";
		
		// TODO Auto-generated method stub
//		PrintWriter out;
//		out = response.getWriter();
		HttpSession sess = request.getSession();
		
		if(request.getParameter("mode").equals("add_voice")) {
			sess.setAttribute(VOICE_SUCCESS, false);
			sess.removeAttribute(VOICE_COMMAND);
			sess.setAttribute(VOICE_COMMAND, request.getParameter("value"));
			System.out.println(sess.getAttribute(VOICE_COMMAND));
			
		} else if(request.getParameter("mode").equals("add_usermenu")) {
			System.out.println(request.getParameter("value"));
			if(!(boolean)sess.getAttribute(VOICE_SUCCESS)) {
				oDb.clear();
				oDb.addTable("search");
				oDb.setInsert("voiceresult", (String)sess.getAttribute(VOICE_COMMAND));
				oDb.setInsert("lastclick", request.getParameter("value"));
				oDb.putData();
			}
		} else if(request.getParameter("mode").equals("voice_success")) {
			sess.setAttribute(VOICE_SUCCESS, true);
			System.out.println("Voice Success");
		}
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		// TODO Auto-generated method stub
	}

}
