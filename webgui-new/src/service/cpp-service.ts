import thrift from 'thrift';
import { LanguageService, FilePosition, Position } from '@thrift-generated';
import { config } from './config';
import { toast } from 'react-toastify';

let client: LanguageService.Client | undefined;
export const createCppClient = (workspace: string) => {
  if (!config) return;
  const connection = thrift.createXHRConnection(config.webserver_host, config.webserver_port, {
    transport: thrift.TBufferedTransport,
    protocol: thrift.TJSONProtocol,
    https: config.webserver_https,
    path: `${config.webserver_path}/${workspace}/CppService`,
  });
  client = thrift.createXHRClient(LanguageService, connection);
  return client;
};

export const getCppFileTypes = async () => {
  if (!client) {
    return [];
  }
  return await client.getFileTypes();
};

export const getCppFileDiagramTypes = async (fileId: string) => {
  let resultMap = new Map<string, number>();
  if (!client) {
    return resultMap;
  }
  try {
    resultMap = await client.getFileDiagramTypes(fileId);
  } catch (e) {
    console.error(e);
    resultMap = new Map();
  }
  return resultMap;
};

export const getCppFileDiagram = async (fileId: string, diagramId: number) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getFileDiagram(fileId, diagramId);
  } catch (e) {
    toast.error('Could not display diagram.');
    console.error(e);
    return '';
  }
};

export const getCppFileDiagramLegend = async (diagramId: number) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getFileDiagramLegend(diagramId);
  } catch (e) {
    toast.error('Could not display diagram legend.');
    console.error(e);
    return '';
  }
};

export const getCppDiagramTypes = async (astNodeId: string) => {
  let resultMap = new Map<string, number>();
  if (!client) {
    return resultMap;
  }
  try {
    resultMap = await client.getDiagramTypes(astNodeId);
  } catch (e) {
    console.error(e);
    resultMap = new Map();
  }
  return resultMap;
};

export const getCppDiagram = async (astNodeId: string, diagramId: number) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getDiagram(astNodeId, diagramId);
  } catch (e) {
    toast.error('Could not display diagram.');
    console.error(e);
    return '';
  }
};

export const getCppDiagramLegend = async (diagramId: number) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getDiagramLegend(diagramId);
  } catch (e) {
    toast.error('Could not display diagram legend.');
    console.error(e);
    return '';
  }
};

export const getCppFileReferenceTypes = async (fileId: string) => {
  let resultMap = new Map<string, number>();
  if (!client) {
    return resultMap;
  }
  try {
    resultMap = await client.getFileReferenceTypes(fileId);
  } catch (e) {
    console.error(e);
    resultMap = new Map();
  }
  return resultMap;
};

export const getCppFileReferences = async (fileId: string, referenceId: number) => {
  if (!client) {
    return [];
  }
  try {
    return await client.getFileReferences(fileId, referenceId);
  } catch (e) {
    console.error(e);
    return [];
  }
};

export const getCppFileReferenceCount = async (fileId: string, referenceId: number) => {
  if (!client) {
    return 0;
  }
  try {
    return await client.getFileReferenceCount(fileId, referenceId);
  } catch (e) {
    console.error(e);
    return 0;
  }
};

export const getCppReferenceTypes = async (astNodeId: string) => {
  let resultMap = new Map<string, number>();
  if (!client) {
    return resultMap;
  }
  try {
    resultMap = await client.getReferenceTypes(astNodeId);
  } catch (e) {
    console.error(e);
    resultMap = new Map();
  }
  return resultMap;
};

export const getCppReferences = async (astNodeId: string, referenceId: number, tags: string[]) => {
  if (!client) {
    return [];
  }
  try {
    return await client.getReferences(astNodeId, referenceId, tags);
  } catch (e) {
    console.error(e);
    return [];
  }
};

export const getCppReferenceCount = async (astNodeId: string, referenceId: number) => {
  if (!client) {
    return 0;
  }
  try {
    return await client.getReferenceCount(astNodeId, referenceId);
  } catch (e) {
    console.error(e);
    return 0;
  }
};

export const getCppReferencesInFile = async (
  astNodeId: string,
  referenceId: number,
  fileId: string,
  tags: string[]
) => {
  if (!client) {
    return [];
  }
  try {
    return await client.getReferencesInFile(astNodeId, referenceId, fileId, tags);
  } catch (e) {
    console.error(e);
    return [];
  }
};

export const getCppReferencesPage = async (
  astNodeId: string,
  referenceId: number,
  pageSize: number,
  pageNo: number
) => {
  if (!client) {
    return [];
  }
  try {
    return await client.getReferencesPage(astNodeId, referenceId, pageSize, pageNo);
  } catch (e) {
    console.error(e);
    return [];
  }
};

export const getCppSourceText = async (astNodeId: string) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getSourceText(astNodeId);
  } catch (e) {
    console.error(e);
    return '';
  }
};

export const getCppProperties = async (astNodeId: string) => {
  let resultMap = new Map<string, string>();
  if (!client) {
    return resultMap;
  }
  try {
    resultMap = await client.getProperties(astNodeId);
  } catch (e) {
    console.error(e);
    resultMap = new Map();
  }
  return resultMap;
};

export const getCppDocumentation = async (astNodeId: string) => {
  if (!client) {
    return '';
  }
  try {
    return await client.getDocumentation(astNodeId);
  } catch (e) {
    toast.error('Could not get documentation about this AST node.');
    console.error(e);
    return '';
  }
};

export const getCppAstNodeInfo = async (astNodeId: string) => {
  if (!client) {
    return;
  }
  try {
    return await client.getAstNodeInfo(astNodeId);
  } catch (e) {
    console.error(e);
    return;
  }
};

export const getCppAstNodeInfoByPosition = async (fileId: string, line: number, column: number) => {
  if (!client) {
    return;
  }
  try {
    return await client.getAstNodeInfoByPosition(
      new FilePosition({
        file: fileId,
        pos: new Position({
          line,
          column,
        }),
      })
    );
  } catch (e) {
    return;
  }
};
