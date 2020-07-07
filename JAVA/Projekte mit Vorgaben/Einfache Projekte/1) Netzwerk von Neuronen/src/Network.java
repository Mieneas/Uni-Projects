/**
 * <NEWLINE>
 * 
 * The Network class implements a neural network.
 * <p>
 * The network consists of three types of neurons: photoreceptors(@see
 * Photoreceptor), interneurons(@see Interneuron) and cortical neurons(@see
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
		if (receptors < 3) {
			throw new RuntimeException(3 - inter + " photoreceptors are missing");
		}
		if (inter < receptors) {
			throw new RuntimeException("the interneurons are less then photoreceptors.");
		}

		//neurons Array erstellen.
		neurons = new Neuron[receptors + inter + cortical];

		//receptors
		this.receptors = receptors;
		for (int i = 0; i < receptors; i++) {
			neurons[i] = new Photoreceptor(i, receptortypes[i % 3]);
		}

		//interneuronen
		for (int i = receptors; i < receptors + inter; i++) {
			neurons[i] = new Interneuron(i - receptors);
		}

		//CorticalNeuron
		this.cortical = cortical;
		for (int i = inter + receptors; i < receptors + inter + cortical; i++) {
			neurons[i] = new CorticalNeuron(i - inter - receptors);
		}


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
		Synapse synapse = new Synapse(n1, n2);
		n1.addSynapse(synapse);
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
		signal = new double[3];
		double[] temp = new double[3];

		//signal wird von receptors empfangen
		for(int i = 0; i < this.receptors; i++){
			 this.neurons[i].integrateSignal(input);
		}

		for(int i = this.neurons.length - this.cortical; i < this.neurons.length; i++) {
			temp = ((CorticalNeuron) this.neurons[i]).getSignal();
			for(int j = 0; j < 3; j ++){
				signal[j] += temp[j];
			}
		}


		double[] colorCount;
		colorCount = new double[3];
		colorCount = countColorreceptors();

		for(int i = 0; i < 3; i++){
			signal[i] /= colorCount[i];
		}

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
		String answer;
		int j = 0;
		double[] signal = new double[3];

			signal[0] = 400;
			signal[1] = 400;
			signal[2] = 400;

			Network net = new Network(50000, 5, 50);
			for (int i = 0; i < net.receptors; i++) {
				net.addSynapse(net.neurons[i], net.neurons[net.receptors + i]);
			}
			for (int i = net.receptors; i < net.neurons.length - net.cortical; i++) {
				if (j == net.cortical) {
					j = 0;
				}
				net.addSynapse(net.neurons[i], net.neurons[(net.neurons.length - net.cortical) + j]);
				j++;
			}
			signal = net.signalprocessing(signal);

			answer = net.colors(signal);

			System.out.println("I see now: " + answer);
	}

}

