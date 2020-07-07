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
        List<Item> notNeeded =  new LinkedList<>();
        int notneededsize;

        for(KitchenItem kI : this.allItems) {
            if(kI != null) {
                notneededsize = kI.getQuantity() - kI.neededquantity;

                for (int i = 0; i < notneededsize; i++) {
                    KitchenItem nN = new KitchenItem(kI);
                    notNeeded.add(nN);
                }
            }else{
                notNeeded.add(null);
            }
        }
        return notNeeded;
    }


    public void putAway() {
        // TODO
        List<Item> temp = new LinkedList<>();

        for(Item k : findSpares())
        {
            temp.add(k);
        }
        Collections.sort(temp);

        for(Item k : temp) {
            if(this.cubby.size() > this.cubbysize) {
                throw new RuntimeException("The cubby is full");
            }
            k.intheCubby = true;
            this.cubby.push(k);
        }
    }

    public void putAwaySmart() {
        // TODO
        List<Item> tempList = new LinkedList<>();
        List<Item> temp = new LinkedList<>();
        for(Item k : findSpares())
        {
            temp.add(k);
        }
        Collections.sort(temp);


        int i = 0;

        while( !(this.cubby.isEmpty()) ) {
            if(this.cubby.lastElement() != null) {
                this.cubby.lastElement().intheCubby = false;
            }
            tempList.add(this.cubby.pop());
        }
        while(!(tempList.isEmpty())) {
            if(this.cubby.size() > this.cubbysize) {
                throw new RuntimeException("The cubby is full.");
            }

            if(((LinkedList<Item>) tempList).getLast() != null) {
                this.cubby.push(((LinkedList<Item>) tempList).getLast());
                this.cubby.lastElement().intheCubby = true;
                ((LinkedList<Item>) tempList).removeLast();
            }
            else {
                if(temp.size() > i) {
                    this.cubby.push(temp.get(i));
                    this.cubby.lastElement().intheCubby = true;
                    i++;
                }
                ((LinkedList<Item>) tempList).removeLast();
            }
        }

        while (this.cubby.size() < this.cubbysize && i < temp.size() ){
            this.cubby.push(temp.get(i));
            this.cubby.lastElement().intheCubby = true;
            i++;
        }
    }

    public boolean replaceable(Item item) {
        // TODO
        return this.cubby.contains(item);
    }

    public Item replace(Item item) {
        // TODO
        Item find;
        List<Item> temp = new LinkedList<>();
        if(item == null) {
            throw new RuntimeException("can not replace null");
        }

        if(replaceable(item)) {
            while ( !(item.equals(cubby.lastElement())) ){
                temp.add(cubby.pop());
            }
            find = new Item(cubby.lastElement().item, cubby.lastElement().getOwner());
            cubby.pop();
            cubby.push(null);

            while ( !(temp.isEmpty()) ){
                cubby.push(((LinkedList<Item>) temp).getLast());
                ((LinkedList<Item>) temp).removeLast();
            }

            return find;
        }
        throw new RuntimeException(item + " is not available");
    }

    public static void main(String[] args) {
        // USE FOR TESTING

        //kitchen
        KitchenItem k1 = new KitchenItem("Teller", 12, "nedal");
        k1.needed(11);
        KitchenItem k2 = new KitchenItem("Teller", 12, "nedal");
        k2.needed(11);

        KitchenItem k3 = new KitchenItem("Löffel", 12, "ziad");
        k3.needed(11);

        KitchenItem k4 = new KitchenItem("Messer", 12, "Abd");
        k4.needed(11);
        KitchenItem k5 = new KitchenItem("Messer", 12, "Abd");
        k5.needed(11);
        KitchenItem k6 = new KitchenItem("Messer", 12, "Abd");
        k6.needed(11);
        KitchenItem k7 = new KitchenItem("Messer", 12, "Abd");
        k7.needed(11);

        KitchenItem k8 = new KitchenItem("Gabel", 12, "Moauea");
        k8.needed(11);
        KitchenItem k9 = new KitchenItem("Gabel", 12, "Moauea");
        k9.needed(11);

        List<KitchenItem> kt = new LinkedList<>();
        kt.add(k1);
        kt.add(k2);
        kt.add(k3);
        kt.add(k4);


        //Stack
        Stack<Item> st = new Stack<>();
        st.push(k1);
        st.lastElement().intheCubby = true;
        st.push(k2);
        st.lastElement().intheCubby = true;
        st.push(k3);
        st.lastElement().intheCubby = true;
        st.push(null);
        st.push(null);
        st.push(k4);
        st.lastElement().intheCubby = true;
        st.push(k5);
        st.lastElement().intheCubby = true;
        st.push(k6);
        st.lastElement().intheCubby = true;
        st.push(k7);
        st.lastElement().intheCubby = true;
        st.push(null);
        st.push(null);
        st.push(null);
        st.push(k8);
        st.lastElement().intheCubby = true;
        st.push(k9);;
        st.lastElement().intheCubby = true;


        ManagetheChaos m = new ManagetheChaos(kt, st);

        /*m.putAway();
        for(Item i : m.cubby){
            System.out.println(i);
        }*/

        m.putAwaySmart();
        for(Item i : m.cubby){
            System.out.println(i);
        }

        System.out.println("replace");
        Item it = m.replace(new Item( "Teller" +
                "", "ziad"));
        System.out.println(it + " is available to replace");




    }
}