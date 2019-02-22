package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;

import javax.swing.JFileChooser;
import javax.swing.JMenuItem;

import org.xml.sax.SAXException;

import tlace.Tlace;
import tlace.TlaceManager;
import tlacegraph.TlaceGraph;
import tlacegraph.TlaceGraphBuilder;

import display.GUI;

/**
 * ImportationListener is a GUI listener responsible for triggering the
 * importation of a counterexample from an XML file.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class ImportationListener implements ActionListener {

	/**
	 * The highest ancestor of this listener.
	 */
	private GUI gui;

	/**
	 * The counterexample manager used to retrieve counterexamples.
	 */
	private TlaceManager manager;

	/**
	 * The menu item used to import a counterexample.
	 */
	private JMenuItem importItem;

	/**
	 * Creates a new importation listener.
	 * 
	 * @param gui
	 *            the GUI this listener listens to.
	 * @param manager
	 *            the counterexample manager used to store and get
	 *            counterexamples.
	 * @param importItem
	 *            the menu item used to import a counterexample.
	 */
	public ImportationListener(GUI gui, TlaceManager manager,
			JMenuItem importItem) {
		this.gui = gui;
		this.manager = manager;
		this.importItem = importItem;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	@Override
	public void actionPerformed(ActionEvent event) {

		// Import menu
		if (event.getSource().equals(this.importItem)) {

			// Trigger the importation

			// Get the file
			JFileChooser chooser = new JFileChooser();
			int returnVal = chooser.showOpenDialog(this.gui);
			if (returnVal == JFileChooser.APPROVE_OPTION) {
				// Get the counterexample
				int cntexId;
				try {
					cntexId = manager.storeCounterexample(chooser
							.getSelectedFile());
					Tlace cntex = manager.getCounterexample(cntexId);
					TlaceGraph graph = TlaceGraphBuilder.buildTlaceGraph(cntex);
					this.gui.display(graph);
				} catch (SAXException e) {
					this.gui.displayError("An error occurred while parsing the given file : "
							+ e.getMessage());
				} catch (IOException e) {
					this.gui.displayError("An error occurred while reading the given file : "
							+ e.getMessage());
				}

			}
		}
	}

}
