public class Item implements Comparable<Item> {
    public String item;
    private String owner;
    public boolean intheCubby;

    public Item(String name, String ownersName) {
        // TODO
        this.item = name;
        this.owner = ownersName;
    }

    public String getOwner() {
        // TODO
        return this.owner;
    }

    public void setOwner(String name) {
        // TODO
        this.owner = name;
    }

    @Override
    public int compareTo(Item o) {
        // TODO
        return this.toString().compareTo(o.toString());
    }

    @Override
    public String toString() {
        // TODO
        return this.owner + ": " + this.item;
    }

    @Override
    public boolean equals(Object obj) {
        // TODO

        if(obj instanceof Item){
            return this.item.equals(((Item) obj).item);
        }
        return false;
    }

    @Override
    public int hashCode() {
        return this.item.hashCode();
    }

}