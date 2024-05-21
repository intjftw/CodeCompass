import { FileInfo } from '@thrift-generated';

export type RouterQueryType = {
  workspaceId?: string;
  projectFileId?: string;
  editorSelection?: string;
  metricsGenId?: string;
  diagramGenId?: string;
  diagramTypeId?: string;
  diagramType?: string;
  languageNodeId?: string;
  gitRepoId?: string;
  gitBranch?: string;
  gitCommitId?: string;
  activeAccordion?: string;
  activeTab?: string;
  treeViewOption?: string;
};

export type TreeNode = {
  info: FileInfo;
  children?: TreeNode[];
};

export type FileNode = {
  [key: string]: {
    expandedNodes: string[];
  };
};

export type SearchProps = {
  initialQuery: string;
  fileSearch: boolean;
  type: number;
  query: string;
  fileFilter: string;
  dirFilter: string;
  start: number;
  size: number;
};
