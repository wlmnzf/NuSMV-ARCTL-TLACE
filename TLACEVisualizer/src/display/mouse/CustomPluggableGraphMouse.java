package display.mouse;

import java.awt.event.MouseEvent;

import edu.uci.ics.jung.visualization.control.CrossoverScalingControl;
import edu.uci.ics.jung.visualization.control.PickingGraphMousePlugin;
import edu.uci.ics.jung.visualization.control.PluggableGraphMouse;
import edu.uci.ics.jung.visualization.control.ScalingGraphMousePlugin;
import edu.uci.ics.jung.visualization.control.TranslatingGraphMousePlugin;

/**
 * This pluggable graph mouse is initiated with different plugins:
 * <ul>
 * <li>picking</li>
 * <li>translating (behind an ALT mask)</li>
 * <li>crossover scaling</li>
 * </ul>
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */
public class CustomPluggableGraphMouse<V, E> extends PluggableGraphMouse {

	/**
	 * Creates a new graph mouse with, as default behaviors, picking, scaling
	 * and translating (behind an ALT mask).
	 */
	public CustomPluggableGraphMouse() {
		super();
		this.add(new PickingGraphMousePlugin<V, E>());
		this.add(new ScalingGraphMousePlugin(new CrossoverScalingControl(), 0));
		this.add(new TranslatingGraphMousePlugin(MouseEvent.ALT_MASK
				| MouseEvent.BUTTON1_MASK));
	}

}
