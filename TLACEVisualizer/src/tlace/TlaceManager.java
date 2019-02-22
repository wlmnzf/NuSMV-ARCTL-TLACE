package tlace;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.xml.sax.SAXException;

import xml.XMLLoader;

/**
 * TlaceManager is a central tree-like annotated counter-examples store.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class TlaceManager {

	/**
	 * The list of TLACEs this manager stores.
	 */
	private List<Tlace> tlaces;

	/**
	 * Creates a new TLACE manager.
	 */
	public TlaceManager() {
		this.tlaces = new ArrayList<Tlace>();
	}

	/**
	 * Stores the counterexample stored in the given XML file. Returns the id of
	 * the stored counterexample.
	 * 
	 * @param XMLfilepath
	 *            the path to a file containing an XML representation of a
	 *            counterexample.
	 * 
	 * @return the id of the stored counterexample.
	 * @throws IOException
	 *             an IOException is thrown if an error occurred while reading
	 *             the given file.
	 * @throws SAXException
	 *             an SAXException is thrown if an error occurred while parsing
	 *             the given file.
	 */
	public int storeCounterexample(String XMLfilepath) throws SAXException,
			IOException {
		return this.storeCounterexample(XMLLoader
				.getCounterexample(XMLfilepath));
	}

	/**
	 * Stores the given counterexample. Returns the id of the stored
	 * counterexample.
	 * 
	 * @param counterexample
	 *            the counterexample to store.
	 * @return the id of the stored counterexample.
	 */
	public int storeCounterexample(Tlace counterexample) {
		this.tlaces.add(counterexample);
		return this.tlaces.size() - 1;
	}

	/**
	 * Stores the counterexample stored in a given file.
	 * 
	 * @param selectedFile
	 *            the file containing the XML representation of a
	 *            counterexample.
	 * @return the id of the stored counterexample.
	 * @throws IOException
	 *             an IOException is thrown if an error occurred while reading
	 *             the given file.
	 * @throws SAXException
	 *             an SAXException is thrown if an error occurred while parsing
	 *             the given file.
	 */
	public int storeCounterexample(File selectedFile) throws SAXException,
			IOException {
		return this.storeCounterexample(XMLLoader
				.getCounterexample(selectedFile));
	}

	/**
	 * Returns the stored counterexample with counterexampleId as id if it
	 * exists, null otherwise.
	 * 
	 * @param counterexampleId
	 *            the id of the requested counterexample.
	 * @return the stored counterexample with counterexampleId as id if it
	 *         exists, null otherwise.
	 */
	public Tlace getCounterexample(int counterexampleId) {
		return (0 <= counterexampleId && counterexampleId < this.tlaces.size()) ? this.tlaces
				.get(counterexampleId) : null;
	}

}
