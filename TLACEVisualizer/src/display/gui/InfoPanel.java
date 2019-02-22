package display.gui;

import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.JPanel;

import tlacegraph.TlaceVertex;
import util.ObservableList;
import display.graph.Vertex;

/**
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public abstract class InfoPanel extends JPanel implements Observer {

	private static final long serialVersionUID = 1L;

	/**
	 * The panel in which this info panel is contained.
	 */
	protected CounterexamplePanel panel;

	/**
	 * Whether or not only display changed variables when displaying a path.
	 */
	protected boolean changedVar;

	/**
	 * Creates a new info panel.
	 */
	public InfoPanel() {
		super();
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		this.panel = null;
		this.changedVar = false;
	}

	/**
	 * Sets the panel to panel.
	 * 
	 * @param panel
	 *            the panel this info panel belongs to.
	 */
	public void setPanel(CounterexamplePanel panel) {
		this.panel = panel;
	}

	/**
	 * Sets whether or not only display changed variables when displaying a path
	 * of states.
	 * 
	 * @param value
	 *            whether or not only display changed variables when displaying
	 *            a path of states.
	 */
	public void setChangedVariablesDisplay(boolean value) {
		this.changedVar = value;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.util.Observer#update(java.util.Observable, java.lang.Object)
	 */
	@SuppressWarnings("unchecked")
	@Override
	public void update(Observable o, Object arg) {
		if (o instanceof ObservableList<?>) {
			ObservableList<Vertex> selectedVertices = (ObservableList<Vertex>) o;

			if (!selectedVertices.isEmpty()) {
				List<TlaceVertex> path = new ArrayList<TlaceVertex>();
				for (Vertex vertex : selectedVertices) {
					path.add(vertex.getVertex());
				}
				this.displayStatesInfo(path);
			} else {
				this.hideInfo();
			}
		}
	}

	/**
	 * Hides the displayed info.
	 */
	private void hideInfo() {
		this.removeAll();
		this.panel.adjustSplitpane();
	}

	/**
	 * Displays the information about the path defined by the given list of
	 * states. states must represent a path in the counterexample.
	 * 
	 * @param states
	 *            the list of states.
	 */
	protected abstract void displayStatesInfo(List<TlaceVertex> states);
}
