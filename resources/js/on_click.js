function click_walm() {
    console.log("clicked!");
    var element = document.getElementById("walm");
    if (element.style.display === "none") {
        element.style.display = "block";
    } else {
        element.style.display = "none";
    }
}