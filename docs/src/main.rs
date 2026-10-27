use askama::Template;
use serde::Deserialize;

#[derive(Deserialize)]
struct ColorPallete {
    colors : Vec<String>
}

#[derive(Template)] 
#[template(path = "index.html")]                         
struct IndexTemplate {
    color_palette : ColorPallete
}


fn main() {

    let color_palette_json = std::fs::read_to_string("data/color_palette.json").unwrap();
    let color_palette: ColorPallete = serde_json::from_str(&color_palette_json).unwrap();

    let index = IndexTemplate { color_palette };

    std::fs::write("index.html", index.render().unwrap())
        .expect("Unable to write to index.html");
}
