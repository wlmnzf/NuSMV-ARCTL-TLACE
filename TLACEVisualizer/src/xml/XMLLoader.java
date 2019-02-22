package xml;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import tlace.Tlace;
import tlace.Node;
import tlace.Transition;
import tlace.Value;
import util.ExpandableList;

/**
 * XMLLoader gives an internal representation of a tree-like annotated
 * counter-example stored in an XML format.
 * 
 * @author Simon Busard <simon.busard@uclouvain.be>
 * 
 */

public class XMLLoader {

	/**
	 * An internal XPath evaluator used to evaluate XPath expressions on XML
	 * representation of counter-examples.
	 */
	private static XPath xpath = XPathFactory.newInstance().newXPath();

	/**
	 * Returns an internal representation of a counter-example stored in an XML
	 * file at filepath.
	 * 
	 * @param filepath
	 *            the path to the file containing the counter-example.
	 * @return an internal representation of the counter-example in the XML file
	 *         at filepath.
	 * @throws IOException
	 *             an IOException is thrown when an error occurred while reading
	 *             the given file.
	 * @throws SAXException
	 *             an SAXException is thrown when an error occurred while
	 *             parsing the given file.
	 */
	public static Tlace getCounterexample(String filepath) throws SAXException,
			IOException {
		Tlace cntex = getCounterexample(new File(filepath));
		return cntex;
	}

	/**
	 * Returns an internal representation of a counter-example stored in a given
	 * XML file.
	 * 
	 * @param file
	 *            the file containing the counter-example.
	 * @return an internal representation of the counter-example stored in file.
	 * @throws IOException
	 *             an IOException is thrown when an error occurred while reading
	 *             the given file.
	 * @throws SAXException
	 *             an SAXException is thrown when an error occurred while
	 *             parsing the given file.
	 */
	public static Tlace getCounterexample(File file) throws SAXException,
			IOException {

		// Open file and get content
		// Gets document builder
		DocumentBuilder builder;
		try {
			builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
			// Reads the file
			Document document = builder.parse(file);

			// Get counterexample
			return getCounterexample(document.getDocumentElement());
		} catch (ParserConfigurationException e) {
			System.err
					.println("XMLLoader.getCounterexample : Problem with parser configuration.");
			e.printStackTrace();
			return null;
		}
	}

	/**
	 * Returns an internal representation of a counter-example given in an XML
	 * format.
	 * 
	 * @param xml
	 *            the XML format of the counter-example.
	 * @return an internal representation of a counter-example given in XML
	 *         format in xml.
	 */
	public static Tlace getCounterexample(Element xml) {
		try {
			String specification = xpath.evaluate("@specification", xml);
			ExpandableList<Node> nodes = new ExpandableList<Node>();
			Node node = getNode(
					(Element) xpath.evaluate("node", xml, XPathConstants.NODE),
					nodes);
			return new Tlace(specification, node);
		} catch (XPathExpressionException e) {
			System.err
					.println("XMLLoader.getCounterexample() : Wrong XPath expression.");
			e.printStackTrace();
			return null;
		}
	}

	/**
	 * Returns an internal representation of a TLACE node given in an XML
	 * format.
	 * 
	 * @param xml
	 *            the XML version of the node.
	 * @param nodes
	 *            the list of all nodes already met during the extraction of the
	 *            counter-example that the node belongs to.
	 * @return
	 */
	private static Node getNode(Element xml, ExpandableList<Node> nodes) {
		try {
			// Get id
			int id = Integer.parseInt(xpath.evaluate("@id", xml));

			// Get values
			Value[] values = getValues((Element) xpath.evaluate("state", xml,
					XPathConstants.NODE));

			// Get atomics
			ArrayList<String> atomics = new ArrayList<String>();
			NodeList xmlatomics = (NodeList) xpath.evaluate("atomic", xml,
					XPathConstants.NODESET);
			for (int i = 0; i < xmlatomics.getLength(); i++) {
				atomics.add(xpath.evaluate("@specification", xmlatomics.item(i)));
			}

			// Get existentials
			Map<String, Node> existentials = new HashMap<String, Node>();
			NodeList xmlexistentials = (NodeList) xpath.evaluate("existential",
					xml, XPathConstants.NODESET);
			Element existential;
			for (int i = 0; i < xmlexistentials.getLength(); i++) {
				existential = (Element) xmlexistentials.item(i);
				if ((Boolean) xpath.evaluate("@explained", existential,
						XPathConstants.BOOLEAN)) {
					existentials.put(
							xpath.evaluate("@specification", existential),
							getPath(existential, nodes));
				} else {
					existentials
							.put(xpath.evaluate("@specification", existential),
									null);
				}

			}

			// Get universals
			ArrayList<String> universals = new ArrayList<String>();
			NodeList xmluniversals = (NodeList) xpath.evaluate("universal",
					xml, XPathConstants.NODESET);
			for (int i = 0; i < xmluniversals.getLength(); i++) {
				universals.add(xpath.evaluate("@specification", xmluniversals.item(i)));
			}

			Node node = new Node(id, values, atomics.toArray(new String[atomics
					.size()]), existentials,
					universals.toArray(new String[universals.size()]));
			nodes.set(id, node);
			return node;

		} catch (NumberFormatException e) {
			System.err.println("XMLLoader.getNode() : id is not a number.");
			e.printStackTrace();
			return null;
		} catch (XPathExpressionException e) {
			System.err.println("XMLLoader.getNode() : Wrong XPath expression.");
			e.printStackTrace();
			return null;
		}
	}

	/**
	 * Returns an internal representation of values given in an XML format.
	 * 
	 * @param xml
	 *            the xml version of the list of values.
	 * @return an array containing all variable-value pairs stored in xml.
	 */
	private static Value[] getValues(Element xml) {
		try {
			ArrayList<Value> values = new ArrayList<Value>();
			NodeList xmlvalues = (NodeList) xpath.evaluate("value", xml,
					XPathConstants.NODESET);
			for (int i = 0; i < xmlvalues.getLength(); i++) {
				values.add(new Value(xpath.evaluate("@variable",
						xmlvalues.item(i)), xpath.evaluate(".",
						xmlvalues.item(i))));
			}
			return values.toArray(new Value[values.size()]);
		} catch (XPathExpressionException e) {
			System.err
					.println("XMLLoader.getValues() : Wrong XPath expression : " + xml);
			e.printStackTrace();
			return null;
		}

	}

	/**
	 * Returns an internal representation of a TLACE path stored in an XML
	 * format.
	 * 
	 * @param xml
	 *            the xml version of the path.
	 * @param nodes
	 *            the set of nodes already seen so far.
	 * @return an internal representation of the TLACE path stored in xml.
	 */
	private static Node getPath(Element xml, ExpandableList<Node> nodes) {
		try {
			Node initial = null;
			Node prev = null;

			// Get XML elements
			NodeList xmlnodes = (NodeList) xpath.evaluate("node", xml,
					XPathConstants.NODESET);
			NodeList xmlinputs = (NodeList) xpath.evaluate("input", xml,
					XPathConstants.NODESET);
			NodeList xmlcombinatorials = (NodeList) xpath.evaluate(
					"combinatorial", xml, XPathConstants.NODESET);

			// Create the path
			Node node;
			for (int i = 0; i < xmlnodes.getLength(); i++) {
				node = getNode((Element) xmlnodes.item(i), nodes);

				if (initial == null) {
					initial = node;
				} else {
					ArrayList<Value> inputs = new ArrayList<Value>();
					for (Value value : getValues((Element) xmlinputs
							.item(i - 1))) {
						inputs.add(value);
					}
					for (Value value : getValues((Element) xmlcombinatorials
							.item(i - 1))) {
						inputs.add(value);
					}

					Transition tr = new Transition(
							inputs.toArray(new Value[inputs.size()]), prev,
							node);
					node.setPrev(tr);
					prev.setNext(tr);
				}

				prev = node;
			}

			// Add loop if necessary
			if ((Boolean) xpath.evaluate("loop", xml, XPathConstants.BOOLEAN)) {
				ArrayList<Value> inputs = new ArrayList<Value>();
				for (Value value : getValues((Element) xmlinputs.item(xmlnodes
						.getLength() - 1))) {
					inputs.add(value);
				}
				for (Value value : getValues((Element) xmlcombinatorials
						.item(xmlnodes.getLength() - 1))) {
					inputs.add(value);
				}

				Element loop = (Element) xpath.evaluate("loop[1]", xml, XPathConstants.NODE);
				Transition tr = new Transition(inputs.toArray(new Value[inputs
						.size()]), prev, nodes.get(Integer.parseInt(xpath
						.evaluate("@to", loop))));
				prev.setNext(tr);
			}

			return initial;

		} catch (XPathExpressionException e) {
			System.err.println("XMLLoader.getPath() : Wrong XPath expression.");
			e.printStackTrace();
			return null;
		}
	}
}
