package display.transformer;

import java.awt.Color;
import java.awt.Paint;

import org.apache.commons.collections15.Transformer;

import display.graph.Vertex;

/**
 * A vertex color transformer is a customized vertex colors transformer that
 * computes, for a given vertex, the filling color. This depends on the
 * explanations of the vertex.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class VertexColorTransformer implements Transformer<Vertex, Paint> {
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.apache.commons.collections15.Transformer#transform(java.lang.Object)
	 */
	@Override
	public Paint transform(Vertex vertex) {
		if (!vertex.getVertex().isFullyExplained()) {
			return Color.RED;
		} else {
			boolean fullyDisplayed = true;
			for (String branch : vertex.getVertex().getBranches().keySet()) {
				if (!vertex.isDisplayed(vertex.getVertex().getBranches()
						.get(branch))) {
					fullyDisplayed = false;
				}
			}
			if (!fullyDisplayed) {
				return Color.ORANGE;
			} else {
				return Color.GREEN;
			}
		}
	}
}
