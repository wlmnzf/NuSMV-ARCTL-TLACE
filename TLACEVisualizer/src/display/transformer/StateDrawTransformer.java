package display.transformer;

import java.awt.Color;
import java.awt.Paint;

import org.apache.commons.collections15.Transformer;

import display.graph.Vertex;

import util.ObservableList;

/**
 * A state draw transformer is a customized node draw transformer that computes,
 * for a given node, the color of the node border. This depends on a list of
 * selected nodes.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class StateDrawTransformer implements Transformer<Vertex, Paint> {

	/**
	 * The set of selected nodes.
	 */
	private ObservableList<Vertex> selectedNodes;

	/**
	 * Creates a new state draw transformer using nodeList as list of nodes.
	 * 
	 * @param nodeList
	 *            the list of nodes to use.
	 */
	public StateDrawTransformer(ObservableList<Vertex> nodeList) {
		this.selectedNodes = nodeList;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.commons.collections15.Transformer#transform(java.lang.Object)
	 */
	@Override
	public Paint transform(Vertex vertex) {
		return (selectedNodes.contains(vertex)) ? Color.BLACK : null;
	}

	/**
	 * Returns whether the given node is in the set of selected nodes or not.
	 * 
	 * @param vertex
	 *            the given node.
	 * @return true if node is in the set of selected nodes, false otherwise.
	 */
	public boolean isSelected(Vertex vertex) {
		return this.selectedNodes.contains(vertex);
	}

	/**
	 * Returns whether the set of selected nodes is empty or not.
	 * 
	 * @return true if the set of selected nodes is empty, false otherwise.
	 */
	public boolean isEmpty() {
		return this.selectedNodes.isEmpty();
	}
}
