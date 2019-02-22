/**
 * 
 */
package display.gui;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.Hashtable;
import java.util.Map;

import javax.swing.ButtonGroup;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.KeyStroke;

import tlace.Value;
import tlacegraph.TlaceGraph;
import util.ObservableList;
import application.Settings;
import display.GUI;
import display.graph.Vertex;
import display.layout.CounterexampleLayout;
import display.listener.ChangedStateVariableListener;
import display.listener.InfoPanelListener;
import display.listener.LayoutListener;
import display.listener.TabListener;
import display.listener.VariableListener;

/**
 * The counterexample panel is a panel displaying a counterexample and all tools
 * related to this counterexample. It includes a menu bar, giving some options
 * specific to the counterexample, the counterexample itself displayed as a
 * graph, etc.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class CounterexamplePanel extends JPanel {

	private static final long serialVersionUID = 1L;

	/**
	 * The displayer of this panel used to display the counterexample.
	 */
	private GraphDisplayer displayer;

	/**
	 * The displayed counterexample.
	 */
	private TlaceGraph counterexample;

	/**
	 * The split panel of this panel, separating the graph panel and the info
	 * panel.
	 */
	private JSplitPane splitPanel;

	/**
	 * The scroll pane of the right part of this split panel.
	 */
	private JScrollPane scrollPane;

	/**
	 * The left panel, displaying counterexamples.
	 */
	private JPanel graphPanel;

	/**
	 * The right panel, displaying state and transition information.
	 */
	private InfoPanel infoPanel;

	private ChangedStateVariableListener changeVarListener;

	/**
	 * Creates a new counterexample panel using displayer to display
	 * counterexample,
	 * 
	 * @param displayer
	 *            the displayer used to display counterexample.
	 * @param infoPanel
	 *            the info panel to use in this panel.
	 * @param counterexample
	 *            the counterexample the new panel will display.
	 * @param gui
	 *            the GUI in which the new panel is.
	 * @param fullyFolded
	 *            whether or not the displayed counter-example have to be open
	 *            fully folded or not.
	 */
	public CounterexamplePanel(GraphDisplayer displayer, InfoPanel infoPanel,
			TlaceGraph counterexample, GUI gui) {
		this.displayer = displayer;
		this.counterexample = counterexample;

		// Creates the GUI

		// Create main panels
		this.setLayout(new BorderLayout(5, 5));
		this.graphPanel = new JPanel();
		this.graphPanel.setLayout(new BorderLayout());
		this.infoPanel = infoPanel;
		this.infoPanel.setPanel(this);

		this.scrollPane = new JScrollPane(this.infoPanel);
		this.scrollPane.setBorder(null);
		this.scrollPane.setMinimumSize(new Dimension(0, this.scrollPane
				.getMinimumSize().height));
		this.splitPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
				this.graphPanel, this.scrollPane);
		this.add(this.splitPanel, BorderLayout.CENTER);
		this.splitPanel.setBorder(null);

		// Sets observable list of selected nodes
		ObservableList<Vertex> selectedNodes = new ObservableList<Vertex>();
		this.displayer.setSelectedNodes(selectedNodes);
		selectedNodes.addObserver(this.infoPanel);

		// Customizes the split pane
		this.splitPanel.setContinuousLayout(true);
		this.splitPanel.setAutoscrolls(true);
		this.splitPanel.setResizeWeight(1);
		this.splitPanel.setOneTouchExpandable(true);
		this.splitPanel.setDividerSize(7);

		// Create the menu
		JMenuBar menubar = new JMenuBar();
		this.add(menubar, BorderLayout.NORTH);

		// File menu
		JMenu fileMenu = new JMenu("File");
		menubar.add(fileMenu);

		// Close menu item
		JMenuItem closeItem = new JMenuItem("Close");
		fileMenu.add(closeItem);
		closeItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W,
				ActionEvent.META_MASK));

		// Options menu
		JMenu optionMenu = new JMenu("Options");
		menubar.add(optionMenu);

		JMenu layoutMenu = new JMenu("Set layout");
		optionMenu.add(layoutMenu);

		// Layout sub menu
		ButtonGroup layouts = new ButtonGroup();
		JRadioButtonMenuItem narrow = new JRadioButtonMenuItem("Narrow layout");
		layoutMenu.add(narrow);
		layouts.add(narrow);
		if (displayer.getLayoutType() == GUI.NARROWTREELIKE) {
			narrow.setSelected(true);
		}
		JRadioButtonMenuItem wide = new JRadioButtonMenuItem("Wide layout");
		// layoutMenu.add(wide); // FIXME fix wide tree-like layout
		// layouts.add(wide); // FIXME fix wide tree-like layout
		if (displayer.getLayoutType() == GUI.WIDETREELIKE) {
			wide.setSelected(true);
		}
		JRadioButtonMenuItem force = new JRadioButtonMenuItem("Force layout");
		layoutMenu.add(force);
		layouts.add(force);
		if (displayer.getLayoutType() == GUI.FORCEBASED) {
			force.setSelected(true);
		}

		// Displayers sub menu
		JMenu infoMenu = new JMenu("Set info panel");
		optionMenu.add(infoMenu);

		ButtonGroup infoPanelGroup = new ButtonGroup();
		JRadioButtonMenuItem treeInfoPanel = new JRadioButtonMenuItem(
				"Tree info panel");
		infoMenu.add(treeInfoPanel);
		infoPanelGroup.add(treeInfoPanel);
		if (this.infoPanel instanceof TreeInfoPanel) {
			treeInfoPanel.setSelected(true);
		}
		JRadioButtonMenuItem tableInfoPanel = new JRadioButtonMenuItem(
				"Table info panel");
		infoMenu.add(tableInfoPanel);
		infoPanelGroup.add(tableInfoPanel);
		if (this.infoPanel instanceof TableInfoPanel) {
			tableInfoPanel.setSelected(true);
		}

		// Get variables
		String[] stateVariables = new String[counterexample.getVertex()
				.getState().length];
		for (int i = 0; i < counterexample.getVertex().getState().length; i++) {
			stateVariables[i] = counterexample.getVertex().getState()[i]
					.getVariable();
		}

		Value[] inputValues;
		if (counterexample.getVertex().getNexts().length > 0) {
			inputValues = counterexample.getVertex().getNexts()[0].getInputs();
		} else {
			inputValues = new Value[0];
		}
		String[] inputVariables = new String[inputValues.length];
		for (int i = 0; i < inputValues.length; i++) {
			inputVariables[i] = inputValues[i].getVariable();
		}

		// Variables sub menu
		JMenu variablesMenu = new JMenu("Display values on graph");
		optionMenu.add(variablesMenu);

		// buttonState is a map giving the corresponding variable to a given
		// checkbox menu item
		Map<JCheckBoxMenuItem, String> buttonState = new Hashtable<JCheckBoxMenuItem, String>();

		// buttonInput is a map giving the corresponding variable to a given
		// checkbox menu item
		Map<JCheckBoxMenuItem, String> buttonInput = new Hashtable<JCheckBoxMenuItem, String>();

		ActionListener variablesListener = new VariableListener(this,
				buttonState, buttonInput, displayer.getVertexTransformer(),
				displayer.getEdgeTransformer());

		// Variables sub menu
		JMenu varState = new JMenu("State variables");
		variablesMenu.add(varState);
		for (String variable : stateVariables) {
			JCheckBoxMenuItem stateVar = new JCheckBoxMenuItem(variable);

			buttonState.put(stateVar, variable);
			stateVar.addActionListener(variablesListener);

			varState.add(stateVar);
		}

		JMenu varInput = new JMenu("Input variables");
		variablesMenu.add(varInput);
		if (inputVariables.length > 0) {
			for (String variable : inputVariables) {
				JCheckBoxMenuItem inputVar = new JCheckBoxMenuItem(variable);

				buttonInput.put(inputVar, variable);
				inputVar.addActionListener(variablesListener);

				// Add listener

				varInput.add(inputVar);
			}
		} else {
			JCheckBoxMenuItem inputVar = new JCheckBoxMenuItem(
					"No input variable");
			inputVar.setEnabled(false);
			varInput.add(inputVar);
		}

		JCheckBoxMenuItem changeVar = new JCheckBoxMenuItem(
				"Show only changed variables");
		optionMenu.add(changeVar);
		changeVarListener = new ChangedStateVariableListener(this.infoPanel,
				selectedNodes);
		changeVar.addActionListener(changeVarListener);

		// Specification
		menubar.add(new JSeparator(JSeparator.VERTICAL));
		menubar.add(new JLabel(this.counterexample.getSpecification()));

		// Link listeners
		closeItem.addActionListener(new TabListener(gui, this));
		LayoutListener displayerListener = new LayoutListener(this, narrow,
				wide, force);
		narrow.addActionListener(displayerListener);
		wide.addActionListener(displayerListener);
		force.addActionListener(displayerListener);
		InfoPanelListener infoPanelListener = new InfoPanelListener(this,
				treeInfoPanel, tableInfoPanel, changeVar);
		treeInfoPanel.addActionListener(infoPanelListener);
		tableInfoPanel.addActionListener(infoPanelListener);

		// Draw the counterexample
		this.redrawCounterexample();
	}

	/**
	 * Sets the displayer to the given one.
	 * 
	 * @param displayer
	 *            the new displayer.
	 */
	public void setDisplayer(GraphDisplayer displayer) {
		this.displayer = displayer;
	}

	/**
	 * Adjusts the location of the divider split pane to display all the path
	 * information.
	 */
	public void adjustSplitpane() {
		this.splitPanel.setDividerLocation(Math.min(
				this.splitPanel.getDividerLocation(),
				this.splitPanel.getSize().width
						- this.infoPanel.getPreferredSize().width
						- Settings.splitPadding));
	}

	/**
	 * Displays the given counterexample on itself.
	 * 
	 * @param component
	 *            the component containing the counterexample to display.
	 */
	public void displayCounterexample(Component component) {
		this.graphPanel.removeAll();
		this.graphPanel.add(component);
		this.splitPanel.setDividerLocation(1.0);
	}

	/**
	 * Redraws the currently displayed counterexample.
	 */
	public void redrawCounterexample() {
		this.displayer.display(this.counterexample, this);
	}

	/**
	 * Sets the graph layout of this panel to layout and redraws the
	 * counterexample.
	 * 
	 * @param layout
	 *            the new layout.
	 */
	public void setGraphLayout(CounterexampleLayout layout) {
		this.displayer.setGraphLayout(layout);
		this.redrawCounterexample();

	}

	/**
	 * Repaints the counterexample.
	 */
	public void repaintCounterexample() {
		this.displayer.repaintCounterexample();
	}

	/**
	 * Returns the displayer used by this panel to display counterexamples.
	 * 
	 * @return the displayer used by this panel to display counterexamples.
	 */
	public GraphDisplayer getDisplayer() {
		return this.displayer;
	}

	/**
	 * Sets this info panel to infoPanel.
	 * 
	 * @param infoPanel
	 *            the new info panel.
	 */
	public void setInfoPanel(InfoPanel infoPanel) {
		this.displayer.getSelectedNodes().deleteObserver(this.infoPanel);

		this.infoPanel = infoPanel;
		this.infoPanel.setPanel(this);
		this.scrollPane.setViewportView(this.infoPanel);
		this.displayer.getSelectedNodes().addObserver(this.infoPanel);
		this.changeVarListener.setInfoPanel(this.infoPanel);

		this.infoPanel.update(this.displayer.getSelectedNodes(), null);

		this.repaint();
	}
}
