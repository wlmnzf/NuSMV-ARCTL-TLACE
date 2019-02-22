package application;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;

/**
 * This class contains all the settings of the application.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class Settings {

	// ----- GENERAL APPLICATION SETTINGS ----------------------------------- //
	/**
	 * The version of the application.
	 */
	public static String applicationVersion = "1.2";

	/**
	 * The name of the application.
	 */
	public static String applicationName = "TLACE Visualizer";

	/**
	 * The title of the application.
	 */
	public static String applicationTitle = applicationName + " "
			+ applicationVersion;

	// ----- GUI SETTINGS --------------------------------------------------- //
	/**
	 * The dimensions of the window of the application.
	 */
	public static Dimension applicationDimension = new Dimension(640, 480);

	/**
	 * The accelerator modifier for shortcuts.
	 */
	public static int acceleratorModifier = (System.getProperty("mrj.version") != null) ? ActionEvent.META_MASK
			: ActionEvent.CTRL_MASK;

	/**
	 * The mask to select graph elements.
	 */
	public static int graphSelectionMask = MouseEvent.BUTTON1_MASK
			| ((System.getProperty("mrj.version") != null) ? MouseEvent.META_MASK
					: MouseEvent.CTRL_MASK);

	/**
	 * The maximum length of tab titles.
	 */
	public static int tabTitleLength = 50;

	/**
	 * The space used to adjust the opening of the splitpane.
	 */
	public static int splitPadding = 30;

	// ----- GRAPH PAINTING SETTINGS ---------------------------------------- //
	/**
	 * The default space between edges of the displayed graphs.
	 */
	public static int vertexSpace = 60;

	/**
	 * The size (width and height) of the empty vertices.
	 */
	public static double vertexSize = 20;

	/**
	 * The internal padding of filled vertices.
	 */
	public static double vertexPadding = 10;

	/**
	 * The padding of the panel containing the graph.
	 */
	public static int graphPadding = 60;

	/**
	 * Espilon value.
	 */
	public static double epsilon = 0.001;

}
