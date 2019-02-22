package display;

import java.awt.event.KeyEvent;

import javax.swing.ButtonGroup;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JTabbedPane;
import javax.swing.KeyStroke;

import tlace.TlaceManager;
import tlacegraph.TlaceGraph;
import application.Settings;
import display.gui.CounterexamplePanel;
import display.gui.GraphDisplayer;
import display.gui.InfoPanel;
import display.gui.TableInfoPanel;
import display.gui.TreeInfoPanel;
import display.layout.CounterexampleLayout;
import display.layout.ForceBasedLayout;
import display.layout.NarrowTreeLikeLayout;
import display.layout.WideTreeLikeLayout;
import display.listener.DefaultDisplayerListener;
import display.listener.DefaultInfoPanelListener;
import display.listener.ImportationListener;
import display.listener.StartFullyFoldedListener;
import display.transformer.StateLabelTransformer;
import display.transformer.TransitionLabelTransformer;

/**
 * GUI is the overall graphical user interface. It contains and manages all the
 * elements of the interface.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class GUI extends JFrame {

	private static final long serialVersionUID = 1L;

	// ----- LAYOUT TYPES --------------------------------------------------- //
	/**
	 * Displayer type of the narrow tree-like layout.
	 */
	public static final int NARROWTREELIKE = 0;

	/**
	 * Displayer type of the wide tree-like layout.
	 */
	public static final int WIDETREELIKE = 1;

	/**
	 * Displayer type of the force-based layout.
	 */
	public static final int FORCEBASED = 2;

	// ----- INFO PANEL TYPES ----------------------------------------------- //
	/**
	 * Info panel type of the tree info panel.
	 */
	public static final int TREEINFOPANEL = 0;

	/**
	 * Info panel type of the table info panel.
	 */
	public static final int TABLEINFOPANEL = 1;

	/**
	 * The current default displayer type.
	 */
	private int displayerType;

	/**
	 * The current info panel type.
	 */
	private int infoPanelType;

	/**
	 * The counterexample manager, used to access stored counterexamples.
	 */
	private TlaceManager manager;

	/**
	 * The tabbed pane displaying counterexamples.
	 */
	private JTabbedPane counterexamplesGUI;

	/**
	 * Whether or not a new counter-example is shown fully folded or not.
	 */
	private boolean startFullyFolded;

	/**
	 * Creates a new GUI based on manager to get and store counterexamples.
	 * Constructs the whole graphical user interface of the application.
	 * 
	 * @param displayerType
	 *            the type of the engine to display a counterexample on the GUI.
	 * @param manager
	 *            the store of counterexamples.
	 */
	public GUI(int displayerType, TlaceManager manager) {
		super(Settings.applicationTitle);
		this.displayerType = displayerType;
		this.infoPanelType = TREEINFOPANEL;
		this.manager = manager;
		this.startFullyFolded = false;

		this.setSize(Settings.applicationDimension);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Create the GUI by adding the adequate components, etc.

		// ----- MENU BAR --------------------------------------------------- //
		// Create the menu bar
		JMenuBar menuBar = new JMenuBar();
		this.setJMenuBar(menuBar);

		// Fill the menu bar
		JMenu fileMenu = new JMenu("File");
		JMenu optionMenu = new JMenu("Options");
		JMenu displayerSubMenu = new JMenu("Set default layout");
		JMenu infoPanelSubMenu = new JMenu("Set default info panel");

		// Create items
		JMenuItem importItem = new JMenuItem("Import XML file");
		importItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_I,
				Settings.acceleratorModifier));

		ButtonGroup displayersGroup = new ButtonGroup();
		JRadioButtonMenuItem narrowDisplayer = new JRadioButtonMenuItem(
				"Narrow tree-like layout");
		displayersGroup.add(narrowDisplayer);
		narrowDisplayer.setSelected(true);
		JRadioButtonMenuItem wideDisplayer = new JRadioButtonMenuItem(
				"Wide tree-like layout");
		//displayersGroup.add(wideDisplayer); // FIXME fix wide tree-like layout
		JRadioButtonMenuItem forceBasedDisplayer = new JRadioButtonMenuItem(
				"Force-based layout");
		displayersGroup.add(forceBasedDisplayer);

		ButtonGroup infoPanelGroup = new ButtonGroup();
		JRadioButtonMenuItem treeInfoPanel = new JRadioButtonMenuItem(
				"Tree info panel");
		infoPanelGroup.add(treeInfoPanel);
		treeInfoPanel.setSelected(true);
		JRadioButtonMenuItem tableInfoPanel = new JRadioButtonMenuItem(
				"Table info panel");
		infoPanelGroup.add(tableInfoPanel);

		JCheckBoxMenuItem fullyFolded = new JCheckBoxMenuItem(
				"Open counter-examples fully folded");

		// Add items
		fileMenu.add(importItem);

		optionMenu.add(displayerSubMenu);
		optionMenu.add(infoPanelSubMenu);
		optionMenu.add(fullyFolded);

		displayerSubMenu.add(narrowDisplayer);
		//displayerSubMenu.add(wideDisplayer); // FIXME fix wide tree-like layout
		displayerSubMenu.add(forceBasedDisplayer);

		infoPanelSubMenu.add(treeInfoPanel);
		infoPanelSubMenu.add(tableInfoPanel);

		menuBar.add(fileMenu);
		menuBar.add(optionMenu);

		// ----- CONTENT ---------------------------------------------------- //
		this.counterexamplesGUI = new JTabbedPane();
		this.getContentPane().add(this.counterexamplesGUI);

		// ----- LISTENERS -------------------------------------------------- //
		importItem.addActionListener(new ImportationListener(this,
				this.manager, importItem));

		DefaultDisplayerListener displayerListener = new DefaultDisplayerListener(
				this, narrowDisplayer, wideDisplayer, forceBasedDisplayer);
		narrowDisplayer.addActionListener(displayerListener);
		wideDisplayer.addActionListener(displayerListener);
		forceBasedDisplayer.addActionListener(displayerListener);

		DefaultInfoPanelListener infoPanelListener = new DefaultInfoPanelListener(
				this, treeInfoPanel, tableInfoPanel);
		treeInfoPanel.addActionListener(infoPanelListener);
		tableInfoPanel.addActionListener(infoPanelListener);

		fullyFolded.addActionListener(new StartFullyFoldedListener(this,
				fullyFolded));

	}

	/**
	 * Displays the given error message in a dedicated dialog.
	 * 
	 * @param error
	 *            the error message to display.
	 */
	public void displayError(String error) {
		JOptionPane.showMessageDialog(this, error, "ERROR",
				JOptionPane.ERROR_MESSAGE);
	}

	/**
	 * Displays the given warning message in a dedicated dialog.
	 * 
	 * @param warning
	 *            the warning message to display.
	 */
	public void displayWarning(String warning) {
		JOptionPane.showMessageDialog(this, warning, "WARNING",
				JOptionPane.WARNING_MESSAGE);
	}

	/**
	 * Sets the default displayer type to type.
	 * 
	 * @param type
	 *            the new type of default displayer.
	 */
	public void setDefaultDisplayer(int type) {
		this.displayerType = type;
	}

	/**
	 * Displays the given counter-example in a new tab.
	 * 
	 * @param cntex
	 *            the graph of the counter-example to display.
	 */
	public void display(TlaceGraph cntex) {
		String tabTitle = (cntex.getSpecification().length() <= Settings.tabTitleLength) ? cntex
				.getSpecification() : cntex.getSpecification().substring(0,
				Math.max(0, Settings.tabTitleLength - 3))
				+ "...";
		GraphDisplayer displayer = getNewDisplayerFromType(this.displayerType);
		displayer.setStartFullyFolded(this.startFullyFolded);
		CounterexamplePanel panel = new CounterexamplePanel(displayer,
				getNewInfoPanelFromType(this.infoPanelType), cntex, this);
		this.counterexamplesGUI.addTab(tabTitle, panel);
		this.counterexamplesGUI.setSelectedIndex(this.counterexamplesGUI
				.getTabCount() - 1);
	}

	/**
	 * Returns a new info panel associated to type.
	 * 
	 * @param type
	 *            the type of the requested info panel.
	 * @return a new info panel of type type if any, a tree info panel
	 *         otherwise.
	 */
	private static InfoPanel getNewInfoPanelFromType(int type) {
		if (type == TREEINFOPANEL) {
			return new TreeInfoPanel();
		} else if (type == TABLEINFOPANEL) {
			return new TableInfoPanel();
		} else {
			return new TreeInfoPanel();
		}
	}

	/**
	 * Closes the given tab.
	 * 
	 * @param panel
	 *            the tab to close.
	 */
	public void close(CounterexamplePanel panel) {
		this.counterexamplesGUI.remove(panel);
	}

	/**
	 * Returns the graph displayer associated to the given displayer type.
	 * 
	 * @param type
	 *            the displayer type.
	 * @return a new graph displayer associated to the given displayer type if
	 *         this type is known, a new graph displayer using narrow tree-like
	 *         layout otherwise.
	 */
	private static GraphDisplayer getNewDisplayerFromType(int type) {
		if (type == GUI.NARROWTREELIKE) {
			return new GraphDisplayer(new NarrowTreeLikeLayout(),
					new StateLabelTransformer(),
					new TransitionLabelTransformer());
		} else if (type == GUI.WIDETREELIKE) {
			return new GraphDisplayer(new WideTreeLikeLayout(),
					new StateLabelTransformer(),
					new TransitionLabelTransformer());
		} else if (type == GUI.FORCEBASED) {
			return new GraphDisplayer(new ForceBasedLayout(),
					new StateLabelTransformer(),
					new TransitionLabelTransformer());
		} else {
			return new GraphDisplayer(new NarrowTreeLikeLayout(),
					new StateLabelTransformer(),
					new TransitionLabelTransformer());
		}
	}

	/**
	 * Returns a layout associated to the given displayer type.
	 * 
	 * @param type
	 *            the displayer type.
	 * @return a layout associated to the given displayer type if this type is
	 *         known, a narrow tree-like layout otherwise.
	 */
	public static CounterexampleLayout getNewLayoutFromType(int type) {
		if (type == GUI.NARROWTREELIKE) {
			return new NarrowTreeLikeLayout();
		} else if (type == GUI.WIDETREELIKE) {
			return new WideTreeLikeLayout();
		} else if (type == GUI.FORCEBASED) {
			return new ForceBasedLayout();
		} else {
			return new NarrowTreeLikeLayout();
		}
	}

	/**
	 * Sets the type of info panel to type.
	 * 
	 * @param type
	 *            the new type of info panels.
	 */
	public void setInfoPanelType(int type) {
		this.infoPanelType = type;
	}

	/**
	 * Sets whether new counter-examples are open fully folded or not.
	 * 
	 * @param fullyFolded
	 *            true if the new counter-examples have to be open fully folded,
	 *            false otherwise.
	 */
	public void setFullyFolded(boolean fullyFolded) {
		this.startFullyFolded = fullyFolded;
	}
}
