public class KitchenItem extends Item {

    private int totalquantity;
    public int neededquantity;

    public KitchenItem(String name, int n) {
        // TODO
        super(name, "shared");
        this.totalquantity = n;
    }

    public KitchenItem(String name, int n, String ownersName) {
        // TODO
        super(name, ownersName);
        this.totalquantity = n;
    }

    public KitchenItem(KitchenItem thing) {
        // TODO
        this(thing.item, thing.totalquantity,thing.getOwner());
        neededquantity = thing.neededquantity;
        intheCubby = thing.intheCubby;

    }

    public int getQuantity() {
        return totalquantity;
    }

    public void needed(int n) {
        neededquantity = n;
    }

}