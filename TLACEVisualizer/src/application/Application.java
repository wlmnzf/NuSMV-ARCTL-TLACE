package application;

import tlace.TlaceManager;
import display.GUI;

/**
 * Application is the entry point of the program. It initiates all the
 * components and launches the GUI.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class Application {

	/**
	 * @param args
	 *            the arguments of the program.
	 */
	public static void main(String[] args) {
		TlaceManager manager = new TlaceManager();
		GUI gui = new GUI(GUI.NARROWTREELIKE, manager);

		gui.setVisible(true);
	}

}
