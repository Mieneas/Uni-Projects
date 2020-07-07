import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Stack;

public class ManagetheChaos {
	List<KitchenItem> allItems;
	Stack<Item> cubby;
	int cubbysize = 15;

	public ManagetheChaos(List<KitchenItem> kitchenItems) {
		allItems = kitchenItems;
		cubby = new Stack<Item>();
	}

	public ManagetheChaos(List<KitchenItem> kitchenItems, Stack C) {
		allItems = kitchenItems;
		if (C.size() > cubbysize)
			throw new RuntimeException("Cubby is not that big!");
		cubby = C;
	}

	public List<Item> findSpares() {
		// TODO
	}

	public void putAway() {
		// TODO
	}

	public void putAwaySmart() {
		// TODO
	}

	public boolean replaceable(Item item) {
		// TODO
	}

	public Item replace(Item item) {
		// TODO
	}

	public static void main(String[] args) {
		// USE FOR TESTING
	}
}

