## TODO/notes

- Crafting system
    - Really this should just be a method to consume an array of items rather than a fully-fledged crafting system
    - ie `ConsumeItem(const TArray<FCItemConsumable>& ConsumedItems, TArray<FCItem>& OutItems);`
- UI
    - I'm not a designer so the system should have at the very least a functional UI.
    - Maybe look around for someone to do some UI for cheap?
- Equippables
    - Items in certain slots should be actually an equipped item
    - Apparel or weapons etc
    - This system doesn't need to implement 
- Inventory Management
    - Item Stacking/Splitting
    - Item Dropping (Project setting to toggle for replicating the drop to other players)
    - Item Destroy (break down items into scrap or something)

___
### Refactor
- Separate dataasset classes
    - Weapons shouldn't share the same asset as a resource like Iron Ore etc
