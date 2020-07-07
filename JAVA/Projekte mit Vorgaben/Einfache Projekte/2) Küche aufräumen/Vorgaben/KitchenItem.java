
public class KitchenItem extends Item {

	private int totalquantity;
	public int neededquantity;

	public KitchenItem(String name, int n) {
		// TODO
	}

	public KitchenItem(String name, int n, String ownersName) {
		// TODO
	}

	public KitchenItem(KitchenItem thing) {
		// TODO
	}

	public int getQuantity() {
		return totalquantity;
	}

	public void needed(int n) {
		neededquantity = n;
	}

}

