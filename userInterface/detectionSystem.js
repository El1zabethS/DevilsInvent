import { GoogleGenAI } from "@google/genai";
import dotenv from "dotenv";

dotenv.config();

const ai = new GoogleGenAI({ apiKey: process.env.GEMINI_API_KEY });

async function main() {
  const response = await ai.models.generateContent({
    model: "gemini-2.5-pro",
    contents: "Here's the UI, which is all outputs from our aurduino robot. Could you analyze the whole UI and tell us of any discripencies you notice and why? \
               For this example, look at the graph in the UI and think what might this reveal about propellors since it tracks oscillations of the compression system. \
              The blue is the EXPECTED values and the yellow is the MEASURED values.",
  });
  console.log(response.text);
}
