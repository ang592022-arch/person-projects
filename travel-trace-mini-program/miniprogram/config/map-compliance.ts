export type MapComplianceScope = 'china' | 'world' | 'taiwan';
export type ProjectUseMode = 'personal-demo' | 'public-release';

export const projectUseMode: ProjectUseMode = 'personal-demo';

export const mapComplianceConfig = {
  chinaBoundarySource: '',
  chinaReviewNumber: '',
  worldBoundarySource: 'Natural Earth (public domain)',
  taiwanBoundarySource: '',
};

export function getMapComplianceNote(scope: MapComplianceScope): string {
  if (scope === 'world') {
    return `World boundaries: ${mapComplianceConfig.worldBoundarySource}`;
  }

  if (projectUseMode === 'personal-demo') {
    return 'Open-source template: China boundary data is not bundled';
  }

  const reviewNumber = mapComplianceConfig.chinaReviewNumber.trim();
  const source = mapComplianceConfig.chinaBoundarySource.trim();
  const reviewText = reviewNumber ? `Review number: ${reviewNumber}` : 'Review number not configured';
  return `China boundaries: ${source || 'source not configured'}; ${reviewText}`;
}
