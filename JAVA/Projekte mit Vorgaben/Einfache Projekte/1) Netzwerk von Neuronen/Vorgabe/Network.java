/**
 * <NEWLINE>
 * 
 * The Network class implements a neural network.
 * <p>
 * The network consists of three types of neurons: photoreceptors(@see
 * Photoreceptor), interneurons(@see Interneuron) and 6 cortical neurons(@see
 * CorticalNeuron). The network processes light waves. There are three types of
 * photoreceptors, that perceive the different colors.
 * 
 * @author Vera Röhr
 * @version 1.0
 * @since 2019-01-11
 */
public class Network {
	/** #Photoreceptors in the network */
	int receptors;
	/** #Cortical neurons in the network */
	int cortical;
	/** All the neurons in the network */
	Neuron[] neurons;
	/** Different receptor types */
	String[] receptortypes = { "blue", "green", "red" };

	/**
	 * Adds neurons to the network.
	 * <p>
	 * Defines the neurons in the network.
	 * 
	 * @param inter
	 *            #Interneurons
	 * @param receptors
	 *            #Photoreceptors
	 * @param cortical
	 *            #CorticalNeurons
	 */
	public Network(int inter, int receptors, int cortical) {
		// TODO
	}

	/**
	 * Add a Synapse between the Neurons. The different neurons have their outgoing
	 * synapses as an attribute. ({@link Interneuron}, {@link Photoreceptor},
	 * {@link CorticalNeuron})
	 * 
	 * @param n1
	 *            Presynaptic Neuron (Sender)
	 * @param n2
	 *            Postsynaptic Neuron (Receiver)
	 */

	public void addSynapse(Neuron n1, Neuron n2) {
		// TODO
	}

	/**
	 * Processes the light waves. The lightwaves are integrated be the
	 * photoreceptors (@see Photoreceptor.integrateSignal(double[] signal)) and the
	 * final neural signal is found by summing up the signals in the cortical
	 * neurons(@see CorticalNeuron)
	 * 
	 * @param input
	 *            light waves
	 * @return the neural signal that can be used to classify the color
	 */
	public double[] signalprocessing(double[] input) {
		double signal[];
		// TODO
		return signal;
	}

	public double[] countColorreceptors() {
		double[] colorreceptors = new double[3];
		Photoreceptor c;
		for (Neuron neuron : this.neurons) {
			if (neuron instanceof Photoreceptor) {
				c = (Photoreceptor) neuron;
				if (c.type == "blue")
					colorreceptors[0]++;
				else if (c.type == "green")
					colorreceptors[1]++;
				else if (c.type == "red")
					colorreceptors[2]++;
			}
		}
		return colorreceptors;
	}

	/**
	 * Classifies the neural signal to a color.
	 * 
	 * @param signal
	 *            neural signal from the cortical neurons
	 * @return color of the mixed light signals as a String
	 */
	public String colors(double[] signal) {
		String color = "grey";
		if (signal[0] > 0.6 && signal[1] < 0.074)
			color = "violet";
		else if (signal[0] > 0.21569 && signal[1] < 0.677)
			color = "blue";
		else if (signal[0] <= 0.21569 && signal[1] > 0.677 && signal[2] > 0.333)
			color = "green";
		else if (signal[1] < 0.713 && signal[2] > 0.913)
			color = "yellow";
		else if (signal[1] > 0.068 && signal[2] > 0.227)
			color = "orange";
		else if (signal[2] > 0.002)
			color = "red";
		return color;
	}

	public static void main(String[] args) {

	}

}

