
public class Item implements Comparable<Item> {
	public String item;
	private String owner;
	public boolean intheCubby;

	public Item(String name, String ownersName) {
		// TODO
	}

	public String getOwner() {
		// TODO
	}

	public void setOwner(String name) {
		// TODO
	}

	@Override
	public int compareTo(Item o) {
		// TODO
	}

	@Override
	public String toString() {
		// TODO
	}

	@Override
	public boolean equals(Object obj) {
		// TODO
	}

	@Override
	public int hashCode() {
		return this.item.hashCode();
	}

}

