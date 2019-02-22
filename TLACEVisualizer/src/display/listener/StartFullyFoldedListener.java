package display.listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JCheckBoxMenuItem;

import display.GUI;

public class StartFullyFoldedListener implements ActionListener {

	private GUI gui;
	private JCheckBoxMenuItem menuItem;

	public StartFullyFoldedListener(GUI gui, JCheckBoxMenuItem fullyFolded) {
		this.gui = gui;
		this.menuItem = fullyFolded;
	}

	@Override
	public void actionPerformed(ActionEvent event) {
		if (event.getSource().equals(this.menuItem)) {
			this.gui.setFullyFolded(this.menuItem.isSelected());
		}
	}

}
