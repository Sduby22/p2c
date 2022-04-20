export class ParseResult {
    success: boolean;
    c_code: string;
    syntax_tree: string;
}

export interface ParserMoudle {
    parse(pascalCode: string): ParseResult;
}

export default function CreateParserModule(mod?: any): Promise<ParserMoudle>;